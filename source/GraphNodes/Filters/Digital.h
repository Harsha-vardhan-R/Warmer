/*
  ==============================================================================

    Digital.h
    Created: 12 May 2024 11:29:06am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <algorithm>
#include <memory>
#include "../GraphNode.h"

#define SQRT_TWO 1.41421356237

// TODO : Extreme Amount of Duplicate code in this file, please cleanup.

class DigitalFilter : public GraphNode {
public:

DigitalFilter(int posX, int posY) : GraphNode(juce::String("Non-Res Filter"), posX, posY),
                                              forwardFFT(10),
                                              inverseFFT(10),
                                              fftOrder(10),
                                              fftSize(1024),
                                              filterCoefficients(fftSize+2, 0.0f),
                                              partStoreBufferIN(2, 256),
                                              partStoreBufferOUT(2, 256),
                                              modulationPartIN(2, 256) {

        // generate and store the window coefficients.
        windowCoefficients.resize(fftSize);
        juce::dsp::WindowingFunction<float>::fillWindowingTables(windowCoefficients.data(), fftSize, juce::dsp::WindowingFunction<float>::hann);

        fftData[0].resize(fftSize, 0.0f);
        fftData[1].resize(fftSize, 0.0f);

        tempData[0].resize(2 * fftSize, 0.0f);
        tempData[1].resize(2 * fftSize, 0.0f);

        overlapStore[0] = new::juce::AudioBuffer<float>(2, fftSize);
        overlapStore[1] = new::juce::AudioBuffer<float>(2, fftSize);
        overlapStore[2] = new::juce::AudioBuffer<float>(2, fftSize);

        std::fill(filterCoefficients.begin(), filterCoefficients.end(), 0.0f);

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(20.0, 20000.0, 440.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mode"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-8.0f, 8.0, 3.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Allow"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.0);


        InputSockets[1]->setSliderCallbackWanted();
        InputSockets[2]->setSliderCallbackWanted();
        InputSockets[3]->setSliderCallbackWanted();

//        InputSockets.add(new GraphNode::Socket(juce::String("Transfer Function"), direction::IN, false));
//        InputSockets[4]->addFilterDisplayControl();
//
//        displayPointer = (filterTransferFunctionDisp*)InputSockets[4]->getPointerToPresentParameterCtrlController();

        needsReloading.store(true);

        makeAllSocketsVisible();

        resized();
    }

    filterTransferFunctionDisp* displayPointer = nullptr;

    void releaseResources() override {}


    void BExp() {
        const int numSamples = bufferToWritePointer->getNumSamples();
        int sampleIndex = 0;
        int sampleIndexRead = 0;

        // Handle leftover samples first
        if (leftOverSamples) {
            float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
            float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

            float* part_out_right = partStoreBufferOUT.getWritePointer(1);
            float* part_out_left = partStoreBufferOUT.getWritePointer(0);

            while (leftOverSamples > 0 && sampleIndex < numSamples) {
                write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];

                sampleIndex++;
                partBufferConsumeIndex++;
                leftOverSamples--;
            }

        }


        while (sampleIndexRead < numSamples) {
            int remainingSamples = numSamples - sampleIndexRead;

            // if there are enough samples to process them and write them to the output.
            if (partBufferFillIndex + remainingSamples >= maxAccumulation) {

                float* part_in_right = partStoreBufferIN.getWritePointer(1);
                float* part_in_left = partStoreBufferIN.getWritePointer(0);
                const float* read_right = readBuff->getReadPointer(1);
                const float* read_left = readBuff->getReadPointer(0);

                int samplesToFill = maxAccumulation - partBufferFillIndex;

                for (int i = 0; i < samplesToFill; i++, sampleIndexRead++, partBufferFillIndex++) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];
                }

                processPartBuffer();

                partBufferFillIndex = 0;
                leftOverSamples = 256;
                partBufferConsumeIndex = 0;

                float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
                float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

                const float* part_out_right = partStoreBufferOUT.getReadPointer(1);
                const float* part_out_left = partStoreBufferOUT.getReadPointer(0);

                // copy the processed samples to the output buffer to write.
                for (; partBufferConsumeIndex < maxAccumulation && sampleIndex < numSamples; sampleIndex++, partBufferConsumeIndex++, leftOverSamples--) {
                    write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                    write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];
                }
            } else {
                // if the samples are not enough just copy them we will process the next time.
                float* part_in_right = partStoreBufferIN.getWritePointer(1);
                float* part_in_left = partStoreBufferIN.getWritePointer(0);

                const float* raw_in_right = readBuff->getWritePointer(1);
                const float* raw_in_left = readBuff->getWritePointer(0);

                for (int i = 0; i < remainingSamples; i++, sampleIndexRead++, partBufferFillIndex++) {
                    part_in_left[partBufferFillIndex] = raw_in_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = raw_in_right[sampleIndexRead];
                }

            }
        }

    }

    void processPartBuffer() {
        float cutoff = InputSockets[1]->getValue() / 19980.0f;
        if (cutoff != prevCutoff) {
            updateFilterCoefficients(cutoff, true);
            prevCutoff = cutoff;
        }

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = partStoreBufferOUT.getWritePointer(channel);
            const float* rawData = partStoreBufferIN.getReadPointer(channel);
            float* fftDataChannel = fftData[channel].data();
            float* tempDataChannel = tempData[channel].data();

            float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
            const float* overlapBuffer2 = overlapStore[1]->getReadPointer(channel);
            const float* overlapBuffer3 = overlapStore[2]->getReadPointer(channel);

            // Shift the previous samples forward by maxAccumulation.
            for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
            }

            // Append maxAccumulation new samples to the end of the fftData buffer.
            for (int i = 0; i < maxAccumulation; ++i) {
                fftDataChannel[fftSize - maxAccumulation + i] = rawData[i];
            }

            // Apply the windowing function to the entire fftSize buffer and copy it to temp buffer.
            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
            }

            // Perform an inplace FFT on the tempDataChannel.
            forwardFFT.performRealOnlyForwardTransform(tempDataChannel);

            // Multiply with the corresponding filter coefficients.
            int half_plus_one = fftSize / 2 + 1;
            for (int i = 0; i < half_plus_one; ++i) {
                float realPart = tempDataChannel[2 * i];
                float imagPart = tempDataChannel[2 * i + 1];
                tempDataChannel[2 * i] = realPart * filterCoefficients[i];
                tempDataChannel[2 * i + 1] = imagPart * filterCoefficients[half_plus_one + i];
            }

            // Perform the inverse FFT
            inverseFFT.performRealOnlyInverseTransform(tempDataChannel);

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = tempDataChannel[i] * windowCoefficients[i];
            }

            // Overlap-add the processed samples into the output buffer
            for (int i = 0; i < maxAccumulation; ++i) {
                channelData[i] = (float)(tempDataChannel[i] +
                                         overlapBuffer3[i + maxAccumulation] +
                                         overlapBuffer2[i + 2 * maxAccumulation] +
                                         overlapBuffer1[i + 3 * maxAccumulation]) * 0.25f;
            }

            // Store the new overlap samples(replace the oldest ones)
            std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
        }

        // change the order of the pointers as
        // from     [ 1, 2, 3 ]
        // to       [ 2, 3, 1 ]
        juce::AudioBuffer<float>* temp = overlapStore[0];
        overlapStore[0] = overlapStore[1];
        overlapStore[1] = overlapStore[2];
        overlapStore[2] = temp;
    }

    const int updateInterval = 16;

    void BExpWithModulation() {
        globalSampleIndex[0] = 0;
        globalSampleIndex[1] = 0;

        const int numSamples = bufferToWritePointer->getNumSamples();
        int sampleIndex = 0;
        int sampleIndexRead = 0;
        juce::AudioBuffer<float>* modulationBuffer = InputSockets[1]->getBufferPointer();

        float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
        float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

        float* part_out_right = partStoreBufferOUT.getWritePointer(1);
        float* part_out_left = partStoreBufferOUT.getWritePointer(0);

        float* part_in_right = partStoreBufferIN.getWritePointer(1);
        float* part_in_left = partStoreBufferIN.getWritePointer(0);

        float* mod_part_in_right = modulationPartIN.getWritePointer(1);
        float* mod_part_in_left = modulationPartIN.getWritePointer(0);

        const float* read_right = readBuff->getReadPointer(1);
        const float* read_left = readBuff->getReadPointer(0);

        const float* mod_read_right = modulationBuffer->getReadPointer(1);
        const float* mod_read_left = modulationBuffer->getReadPointer(0);

        // Handle leftover samples first
        while (leftOverSamples > 0 && sampleIndex < numSamples) {
            write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
            write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];

            sampleIndex++;
            partBufferConsumeIndex++;
            leftOverSamples--;
        }

        while (sampleIndexRead < numSamples) {
            int remainingSamples = numSamples - sampleIndexRead;

            if (partBufferFillIndex + remainingSamples >= maxAccumulation) {
                int samplesToFill = maxAccumulation - partBufferFillIndex;

                for (int i = 0; i < samplesToFill; ++i, ++sampleIndexRead, ++partBufferFillIndex) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];

                    mod_part_in_left[partBufferFillIndex] = mod_read_left[sampleIndexRead];
                    mod_part_in_right[partBufferFillIndex] = mod_read_right[sampleIndexRead];
                }

                processPartBufferWithModulation(modulationBuffer);

                partBufferFillIndex = 0;
                leftOverSamples = maxAccumulation;
                partBufferConsumeIndex = 0;

                for (; partBufferConsumeIndex < maxAccumulation && sampleIndex < numSamples; ++sampleIndex, ++partBufferConsumeIndex, --leftOverSamples) {
                    write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                    write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];
                }
            } else {
                for (int i = 0; i < remainingSamples; ++i, ++sampleIndexRead, ++partBufferFillIndex) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];

                    mod_part_in_left[partBufferFillIndex] = mod_read_left[sampleIndexRead];
                    mod_part_in_right[partBufferFillIndex] = mod_read_right[sampleIndexRead];
                }
            }
        }
    }

    void processPartBufferWithModulation(juce::AudioBuffer<float>* modulationData) {
        for (int channel = 0; channel < 2; ++channel) {
            int ModPartBufferIndex = -1;

            float* channelData = partStoreBufferOUT.getWritePointer(channel);
            const float* rawData = partStoreBufferIN.getReadPointer(channel);
            float* fftDataChannel = fftData[channel].data();
            float* tempDataChannel = tempData[channel].data();

            float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
            const float* overlapBuffer2 = overlapStore[1]->getReadPointer(channel);
            const float* overlapBuffer3 = overlapStore[2]->getReadPointer(channel);

            const float* cutoffModulation = modulationPartIN.getReadPointer(channel);

            for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
            }

            for (int i = 0; i < maxAccumulation; ++i) {
                fftDataChannel[fftSize - maxAccumulation + i] = rawData[i];
            }

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
            }

            forwardFFT.performRealOnlyForwardTransform(tempDataChannel);

            int half_plus_one = fftSize / 2 + 1;
            for (int i = 0; i < half_plus_one; ++i) {
                if (i % 2 == 0) {
                    ++ModPartBufferIndex;
                    if (i % 17 == 0) updateFilterCoefficients(cutoffModulation[ModPartBufferIndex], false);
                }
                float val = filterCoefficients[i];
                tempDataChannel[2 * i] *= val;
                tempDataChannel[2 * i + 1] *= val;
            }

            inverseFFT.performRealOnlyInverseTransform(tempDataChannel);

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = tempDataChannel[i] * windowCoefficients[i];
            }

            for (int i = 0; i < maxAccumulation; ++i) {
                channelData[i] = (tempDataChannel[i] +
                                  overlapBuffer3[i + maxAccumulation] +
                                  overlapBuffer2[i + 2 * maxAccumulation] +
                                  overlapBuffer1[i + 3 * maxAccumulation]) * 0.25f;
            }

            std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
        }

        juce::AudioBuffer<float>* temp = overlapStore[0];
        overlapStore[0] = overlapStore[1];
        overlapStore[1] = overlapStore[2];
        overlapStore[2] = temp;
    }



    // while processing we calculate the fft for 1024 samples
    // at a time, and we process for every 256 samples.
    //
    // that is we always have a 75% overlap,
    // for 512 samples we need to run this twice for a block
    // and for 64 samples we wait till they get accumulated till
    // 256 samples.
    void processGraphNode() override {
        (this->*callback_pointer)();
    }

    void updateFilterCoefficients(float cutoff, bool update) {
        float mode = InputSockets[2]->getValue();
        float mix = InputSockets[3]->getValue();

        float cutoffFrequency = std::clamp(cutoff * 19980.0f, 20.0f, 20000.0f);

        int half_plus_one = fftSize / 2 + 1;

        for (int i = 0; i < half_plus_one; ++i) {
            float frequency = (float)i * ((float)sampleRate / (float)fftSize);

            float s = frequency / cutoffFrequency;
            float power;

            if (mode >= 0.0f) power = std::pow(s , mode);
            else power = 1.0f / std::pow(s , std::abs(mode));

            float val = safeClamp(( mix * power + 1.0f ) / ( power + 1.0f ), 0.0f, 1.0f);

            // phase delay is causing interesting sounds but causing
            // too much variation.
            filterCoefficients[i] = val;
//            filterCoefficients[half_plus_one + i] = val;

        }

    }


    static float safeClamp(float value, float lower, float upper) {
        if (std::isnan(value)) { return lower; }
        return std::clamp(value, lower, upper);
    }

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        accumulation = 0;
        readBuff = InputSockets[0]->getBufferPointer();

        for (auto i : overlapStore) i->clear();

        partBufferFillIndex = 0;
        partBufferConsumeIndex = 0;

        prevCutoff = -1.0f;

        leftOverSamples = 256;

        partStoreBufferOUT.setNotClear();
        partStoreBufferOUT.clear();

        if (InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            modulationPartIN.clear();
            callback_pointer = &DigitalFilter::BExpWithModulation;
        } else {
            callback_pointer = &DigitalFilter::BExp;
        }
    }

    void mini_reset() override {
        updateFilterCoefficients(InputSockets[1]->getValue() / 20000.0f, true);
    }

    ~DigitalFilter() override {
        for (auto i : overlapStore) delete i;
    }

private:

    void (DigitalFilter::*callback_pointer)() = nullptr;

    std::atomic<bool> needsReloading{};

    juce::dsp::FFT forwardFFT, inverseFFT;

    int oo = 1;

    const int fftSize;
    const int fftOrder;

    // fft data is where the previous data is stored.
    // temp data is where it is transferred for the inplace FFT operation.
    std::vector<float> fftData[2], tempData[2];

    // This is going to have the size of 1026 as we need to have both real and
    // imaginary values for the H(s). and we need value only for 0->512 indexes,
    // the remaining are just the conjugates.
    juce::AudioBuffer<float> *readBuff = nullptr, partStoreBufferIN, partStoreBufferOUT, modulationPartIN;
    int partBufferFillIndex = 0, partBufferConsumeIndex = 0;
    int leftOverSamples = 256;
    int globalSampleIndex[2] = { 0, 0 };

    int accumulation = 0;

    std::vector<float> windowCoefficients, filterCoefficients;

    const int maxAccumulation = 256;

    juce::AudioBuffer<float>* overlapStore[3] = { nullptr, nullptr , nullptr };

    float prevCutoff = -1.0f;

};


// Resonant Filter.

class DigitalFilterResonant : public GraphNode {
public:

DigitalFilterResonant(int posX, int posY) : GraphNode(juce::String("Resonant Filter"), posX, posY),
                                              forwardFFT(10),
                                              inverseFFT(10),
                                              fftOrder(10),
                                              fftSize(1024),
                                              filterCoefficients(fftSize+2, 0.0f),
                                              partStoreBufferIN(2, 256),
                                              partStoreBufferOUT(2, 256),
                                              modulationPartIN(2, 256) {

        // generate and store the window coefficients.
        windowCoefficients.resize(fftSize);
        juce::dsp::WindowingFunction<float>::fillWindowingTables(windowCoefficients.data(), fftSize, juce::dsp::WindowingFunction<float>::hann);

        fftData[0].resize(fftSize, 0.0f);
        fftData[1].resize(fftSize, 0.0f);

        tempData[0].resize(2 * fftSize, 0.0f);
        tempData[1].resize(2 * fftSize, 0.0f);

        overlapStore[0] = new::juce::AudioBuffer<float>(2, fftSize);
        overlapStore[1] = new::juce::AudioBuffer<float>(2, fftSize);
        overlapStore[2] = new::juce::AudioBuffer<float>(2, fftSize);

        std::fill(filterCoefficients.begin(), filterCoefficients.end(), 0.0f);

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(20.0, 20000.0, 440.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mode"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-8.0f, 8.0, 3.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Allow"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Resonance"), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(0.0, 1.0, 0.0);
        
        InputSockets[1]->setSliderCallbackWanted();
        InputSockets[2]->setSliderCallbackWanted();
        InputSockets[3]->setSliderCallbackWanted();
        InputSockets[4]->setSliderCallbackWanted();

//        InputSockets.add(new GraphNode::Socket(juce::String("Transfer Function"), direction::IN, false));
//        InputSockets[4]->addFilterDisplayControl();
//
//        displayPointer = (filterTransferFunctionDisp*)InputSockets[4]->getPointerToPresentParameterCtrlController();

        needsReloading.store(true);

        makeAllSocketsVisible();

        resized();
    }

    filterTransferFunctionDisp* displayPointer = nullptr;

    void releaseResources() override {}

    void BExp() {
        const int numSamples = bufferToWritePointer->getNumSamples();
        int sampleIndex = 0;
        int sampleIndexRead = 0;

        // Handle leftover samples first
        if (leftOverSamples) {
            float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
            float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

            float* part_out_right = partStoreBufferOUT.getWritePointer(1);
            float* part_out_left = partStoreBufferOUT.getWritePointer(0);

            while (leftOverSamples > 0 && sampleIndex < numSamples) {
                write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];

                sampleIndex++;
                partBufferConsumeIndex++;
                leftOverSamples--;
            }

        }


        while (sampleIndexRead < numSamples) {
            int remainingSamples = numSamples - sampleIndexRead;

            // if there are enough samples to process them and write them to the output.
            if (partBufferFillIndex + remainingSamples >= maxAccumulation) {

                float* part_in_right = partStoreBufferIN.getWritePointer(1);
                float* part_in_left = partStoreBufferIN.getWritePointer(0);
                const float* read_right = readBuff->getReadPointer(1);
                const float* read_left = readBuff->getReadPointer(0);

                int samplesToFill = maxAccumulation - partBufferFillIndex;

                for (int i = 0; i < samplesToFill; i++, sampleIndexRead++, partBufferFillIndex++) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];
                }

                processPartBuffer();

                partBufferFillIndex = 0;
                leftOverSamples = 256;
                partBufferConsumeIndex = 0;

                float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
                float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

                const float* part_out_right = partStoreBufferOUT.getReadPointer(1);
                const float* part_out_left = partStoreBufferOUT.getReadPointer(0);

                // copy the processed samples to the output buffer to write.
                for (; partBufferConsumeIndex < maxAccumulation && sampleIndex < numSamples; sampleIndex++, partBufferConsumeIndex++, leftOverSamples--) {
                    write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                    write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];
                }
            } else {
                // if the samples are not enough just copy them we will process the next time.
                float* part_in_right = partStoreBufferIN.getWritePointer(1);
                float* part_in_left = partStoreBufferIN.getWritePointer(0);

                const float* raw_in_right = readBuff->getWritePointer(1);
                const float* raw_in_left = readBuff->getWritePointer(0);

                for (int i = 0; i < remainingSamples; i++, sampleIndexRead++, partBufferFillIndex++) {
                    part_in_left[partBufferFillIndex] = raw_in_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = raw_in_right[sampleIndexRead];
                }

            }
        }

    }

    void processPartBuffer() {
        float cutoff = InputSockets[1]->getValue() / 19980.0f;
        if (cutoff != prevCutoff) {
            updateFilterCoefficients(cutoff, true);
            prevCutoff = cutoff;
        }

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = partStoreBufferOUT.getWritePointer(channel);
            const float* rawData = partStoreBufferIN.getReadPointer(channel);
            float* fftDataChannel = fftData[channel].data();
            float* tempDataChannel = tempData[channel].data();

            float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
            const float* overlapBuffer2 = overlapStore[1]->getReadPointer(channel);
            const float* overlapBuffer3 = overlapStore[2]->getReadPointer(channel);

            // Shift the previous samples forward by maxAccumulation.
            for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
            }

            // Append maxAccumulation new samples to the end of the fftData buffer.
            for (int i = 0; i < maxAccumulation; ++i) {
                fftDataChannel[fftSize - maxAccumulation + i] = rawData[i];
            }

            // Apply the windowing function to the entire fftSize buffer and copy it to temp buffer.
            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
            }

            // Perform an inplace FFT on the tempDataChannel.
            forwardFFT.performRealOnlyForwardTransform(tempDataChannel);

            // Multiply with the corresponding filter coefficients.
            int half_plus_one = fftSize / 2 + 1;
            for (int i = 0; i < half_plus_one; ++i) {
                float realPart = tempDataChannel[2 * i];
                float imagPart = tempDataChannel[2 * i + 1];
                tempDataChannel[2 * i] = realPart * filterCoefficients[i];
                tempDataChannel[2 * i + 1] = imagPart * filterCoefficients[half_plus_one + i];
            }

            // Perform the inverse FFT
            inverseFFT.performRealOnlyInverseTransform(tempDataChannel);

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = tempDataChannel[i] * windowCoefficients[i];
            }

            // Overlap-add the processed samples into the output buffer
            for (int i = 0; i < maxAccumulation; ++i) {
                channelData[i] = (float)(tempDataChannel[i] +
                                         overlapBuffer3[i + maxAccumulation] +
                                         overlapBuffer2[i + 2 * maxAccumulation] +
                                         overlapBuffer1[i + 3 * maxAccumulation]) * 0.25f;
            }

            // Store the new overlap samples(replace the oldest ones)
            std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
        }

        // change the order of the pointers as
        // from     [ 1, 2, 3 ]
        // to       [ 2, 3, 1 ]
        juce::AudioBuffer<float>* temp = overlapStore[0];
        overlapStore[0] = overlapStore[1];
        overlapStore[1] = overlapStore[2];
        overlapStore[2] = temp;
    }

    const int updateInterval = 16;

    void BExpWithModulation() {
        globalSampleIndex[0] = 0;
        globalSampleIndex[1] = 0;

        const int numSamples = bufferToWritePointer->getNumSamples();
        int sampleIndex = 0;
        int sampleIndexRead = 0;
        juce::AudioBuffer<float>* modulationBuffer = InputSockets[1]->getBufferPointer();

        float* write_right_ch_data = bufferToWritePointer->getWritePointer(1);
        float* write_left_ch_data = bufferToWritePointer->getWritePointer(0);

        float* part_out_right = partStoreBufferOUT.getWritePointer(1);
        float* part_out_left = partStoreBufferOUT.getWritePointer(0);

        float* part_in_right = partStoreBufferIN.getWritePointer(1);
        float* part_in_left = partStoreBufferIN.getWritePointer(0);

        float* mod_part_in_right = modulationPartIN.getWritePointer(1);
        float* mod_part_in_left = modulationPartIN.getWritePointer(0);

        const float* read_right = readBuff->getReadPointer(1);
        const float* read_left = readBuff->getReadPointer(0);

        const float* mod_read_right = modulationBuffer->getReadPointer(1);
        const float* mod_read_left = modulationBuffer->getReadPointer(0);

        // Handle leftover samples first
        while (leftOverSamples > 0 && sampleIndex < numSamples) {
            write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
            write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];

            sampleIndex++;
            partBufferConsumeIndex++;
            leftOverSamples--;
        }

        while (sampleIndexRead < numSamples) {
            int remainingSamples = numSamples - sampleIndexRead;

            if (partBufferFillIndex + remainingSamples >= maxAccumulation) {
                int samplesToFill = maxAccumulation - partBufferFillIndex;

                for (int i = 0; i < samplesToFill; ++i, ++sampleIndexRead, ++partBufferFillIndex) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];

                    mod_part_in_left[partBufferFillIndex] = mod_read_left[sampleIndexRead];
                    mod_part_in_right[partBufferFillIndex] = mod_read_right[sampleIndexRead];
                }

                processPartBufferWithModulation(modulationBuffer);

                partBufferFillIndex = 0;
                leftOverSamples = maxAccumulation;
                partBufferConsumeIndex = 0;

                for (; partBufferConsumeIndex < maxAccumulation && sampleIndex < numSamples; ++sampleIndex, ++partBufferConsumeIndex, --leftOverSamples) {
                    write_right_ch_data[sampleIndex] = part_out_right[partBufferConsumeIndex];
                    write_left_ch_data[sampleIndex] = part_out_left[partBufferConsumeIndex];
                }
            } else {
                for (int i = 0; i < remainingSamples; ++i, ++sampleIndexRead, ++partBufferFillIndex) {
                    part_in_left[partBufferFillIndex] = read_left[sampleIndexRead];
                    part_in_right[partBufferFillIndex] = read_right[sampleIndexRead];

                    mod_part_in_left[partBufferFillIndex] = mod_read_left[sampleIndexRead];
                    mod_part_in_right[partBufferFillIndex] = mod_read_right[sampleIndexRead];
                }
            }
        }
    } 

    void processPartBufferWithModulation(juce::AudioBuffer<float>* modulationData) {
        for (int channel = 0; channel < 2; ++channel) {
            int ModPartBufferIndex = -1;

            float* channelData = partStoreBufferOUT.getWritePointer(channel);
            const float* rawData = partStoreBufferIN.getReadPointer(channel);
            float* fftDataChannel = fftData[channel].data();
            float* tempDataChannel = tempData[channel].data();

            float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
            const float* overlapBuffer2 = overlapStore[1]->getReadPointer(channel);
            const float* overlapBuffer3 = overlapStore[2]->getReadPointer(channel);

            const float* cutoffModulation = modulationPartIN.getReadPointer(channel);

            for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
            }

            for (int i = 0; i < maxAccumulation; ++i) {
                fftDataChannel[fftSize - maxAccumulation + i] = rawData[i];
            }

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
            }

            forwardFFT.performRealOnlyForwardTransform(tempDataChannel);

            int half_plus_one = fftSize / 2 + 1;
            for (int i = 0; i < half_plus_one; ++i) {
                if (i % 2 == 0) {
                    ++ModPartBufferIndex;
                    if (i % 17 == 0) updateFilterCoefficients(cutoffModulation[ModPartBufferIndex], false);
                }
                float val = filterCoefficients[i];
                tempDataChannel[2 * i] *= val;
                tempDataChannel[2 * i + 1] *= val;
            }

            inverseFFT.performRealOnlyInverseTransform(tempDataChannel);

            for (int i = 0; i < fftSize; ++i) {
                tempDataChannel[i] = tempDataChannel[i] * windowCoefficients[i];
            }

            for (int i = 0; i < maxAccumulation; ++i) {
                channelData[i] = (tempDataChannel[i] +
                                  overlapBuffer3[i + maxAccumulation] +
                                  overlapBuffer2[i + 2 * maxAccumulation] +
                                  overlapBuffer1[i + 3 * maxAccumulation]) * 0.25f;
            }

            std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
        }

        juce::AudioBuffer<float>* temp = overlapStore[0];
        overlapStore[0] = overlapStore[1];
        overlapStore[1] = overlapStore[2];
        overlapStore[2] = temp;
    }



    // while processing we calculate the fft for 1024 samples
    // at a time, and we process for every 256 samples.
    //
    // that is we always have a 75% overlap,
    // for 512 samples we need to run this twice for a block
    // and for 64 samples we wait till they get accumulated till
    // 256 samples.
    void processGraphNode() override {
        (this->*callback_pointer)();
    }

    void updateFilterCoefficients(float cutoff, bool update) {
        float mode = InputSockets[2]->getValue();
        float mix = InputSockets[3]->getValue();
        float reso = std::clamp(1.0f-InputSockets[4]->getValue(), 0.01f, 1.0f);
        
        float cutoffFrequency = std::clamp(cutoff * 19980.0f, 20.0f, 20000.0f);

        int half_plus_one = fftSize / 2 + 1;

        for (int i = 0; i < half_plus_one; ++i) {
            float frequency = (float)i * ((float)sampleRate / (float)fftSize);

            float s = frequency / cutoffFrequency;
            float power;

            if (mode >= 0.0f) {
                power = std::pow(s, mode);
            } else {
                power = 1.0f / std::pow(s, std::abs(mode));
            }

            float val = safeClamp((mix * power + 1.0f) / (power + 1.0f), 0.0f, 1.0f);

            float resonanceFactor = std::pow(10.0f, reso); 
            float resonanceGain = resonanceFactor / (s * s + resonanceFactor);

            // Blend resonance with original value
            val = val * (1.0f - reso) + resonanceGain * reso;

            filterCoefficients[i] = val;
        }

    }


    static float safeClamp(float value, float lower, float upper) {
        if (std::isnan(value)) { return lower; }
        return std::clamp(value, lower, upper);
    }

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        accumulation = 0;
        readBuff = InputSockets[0]->getBufferPointer();

        for (auto i : overlapStore) i->clear();

        partBufferFillIndex = 0;
        partBufferConsumeIndex = 0;

        prevCutoff = -1.0f;

        leftOverSamples = 256;

        partStoreBufferOUT.setNotClear();
        partStoreBufferOUT.clear();


        if (InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            modulationPartIN.clear();
            callback_pointer = &DigitalFilterResonant::BExpWithModulation;
        } else {
            callback_pointer = &DigitalFilterResonant::BExp;
        }
    }

    void mini_reset() override {
        updateFilterCoefficients(InputSockets[1]->getValue() / 20000.0f, true);
    }

    ~DigitalFilterResonant() override {
        for (auto i : overlapStore) delete i;
    }

private:

    void (DigitalFilterResonant::*callback_pointer)() = nullptr;

    std::atomic<bool> needsReloading{};

    juce::dsp::FFT forwardFFT, inverseFFT;

    int oo = 1;

    const int fftSize;
    const int fftOrder;

    // fft data is where the previous data is stored.
    // temp data is where it is transferred for the inplace FFT operation.
    std::vector<float> fftData[2], tempData[2];

    // This is going to have the size of 1026 as we need to have both real and
    // imaginary values for the H(s). and we need value only for 0->512 indexes,
    // the remaining are just the conjugates.
    juce::AudioBuffer<float> *readBuff = nullptr, partStoreBufferIN, partStoreBufferOUT, modulationPartIN;
    int partBufferFillIndex = 0, partBufferConsumeIndex = 0;
    int leftOverSamples = 256;
    int globalSampleIndex[2] = { 0, 0 };

    int accumulation = 0;

    std::vector<float> windowCoefficients, filterCoefficients;

    const int maxAccumulation = 256;

    juce::AudioBuffer<float>* overlapStore[3] = { nullptr, nullptr , nullptr };

    float prevCutoff = -1.0f;

};
