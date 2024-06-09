/*
  ==============================================================================

    Digital.h
    Created: 12 May 2024 11:29:06am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <memory>
#include "../GraphNode.h"

#define SQRT_TWO 1.41421356237


class DigitalFilter : public GraphNode {
public:

    DigitalFilter(int posX, int posY) : GraphNode(juce::String("simple filter"), posX, posY), forwardFFT(10),
                                                                inverseFFT(10), fftOrder(10), fftSize(1024),
                                                                filterCoefficients(fftSize, 0.0f),
                                                                partStoreBufferIN(2, 256), partStoreBufferOUT(2, 256) {

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

        std::fill(filterCoefficients.begin(), filterCoefficients.end(), 1.0f);

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(20.0, 20000.0, 440.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mode"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-8.0f, 8.0, 3.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mix"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.0);

        InputSockets[1]->setSilderCallbackWanted();
        InputSockets[2]->setSilderCallbackWanted();
        InputSockets[3]->setSilderCallbackWanted();

        InputSockets.add(new GraphNode::Socket(juce::String("Transfer Function"), direction::IN, false));
        InputSockets[4]->addFilterDisplayControl();

        displayPointer = (filterTransferFunctionDisp*)InputSockets[4]->getPointerToPresentParameterCtrlController();

        needsReloading.store(true);

        makeAllSocketsVisible();

        resized();
    }

    filterTransferFunctionDisp* displayPointer = nullptr;

    void releaseResources() override {}


    void BiggerOrEqualBufferSize() {
        const int numSamples = bufferToWritePointer->getNumSamples();

        accumulation += numSamples;
        int sampleIndex = 0;

        bufferToWritePointer->clear();

        while ((accumulation - maxAccumulation) >= 0) {
            float cutoff = InputSockets[1]->getValue() / 20000.0f;
            if (cutoff != prevCutoff) {
                updateFilterCoefficients(cutoff);
                prevCutoff = cutoff;
            }

            for (int channel = 0; channel < 2; ++channel) {
                float* channelData = bufferToWritePointer->getWritePointer(channel);
                const float* rawData = readBuff->getReadPointer(channel);
                float* fftDataChannel = fftData[channel].data();
                float* tempDataChannel = tempData[channel].data();

                float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
                float* overlapBuffer2 = overlapStore[1]->getWritePointer(channel);
                float* overlapBuffer3 = overlapStore[2]->getWritePointer(channel);

                // Shift the previous samples forward by maxAccumulation.
                for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                    fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
                }

                // Append maxAccumulation new samples to the end of the fftData buffer.
                for (int i = 0; i < maxAccumulation; ++i) {
                    fftDataChannel[fftSize - maxAccumulation + i] = rawData[sampleIndex + i];
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
                    tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
                }

                // Overlap-add the processed samples into the output buffer
                for (int i = 0; i < maxAccumulation; ++i) {
                    channelData[sampleIndex + i] += (float)(tempDataChannel[i] +
                                                            overlapBuffer3[i + maxAccumulation] +
                                                            overlapBuffer2[i + 2 * maxAccumulation] +
                                                            overlapBuffer1[i + 3 * maxAccumulation]) * 0.666f;
                }

                // Store the new overlap samples
                std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
            }

            // change the order of the pointers as
            // from     [ 1, 2, 3 ]
            // to       [ 2, 3, 1 ]
            juce::AudioBuffer<float>* temp = overlapStore[0];
            overlapStore[0] = overlapStore[1];
            overlapStore[1] = overlapStore[2];
            overlapStore[2] = temp;

            sampleIndex += maxAccumulation;
            accumulation -= maxAccumulation;
        }
    }


    // this method should work for any smaller buffer size.
    void Generalised() {
        const int numSamples = bufferToWritePointer->getNumSamples();
        accumulation += numSamples;
        int sampleIndex = 0;

        while (accumulation >= maxAccumulation) {
            float cutoff = InputSockets[1]->getValue() / 20000.0f;
            if (cutoff != prevCutoff) {
                updateFilterCoefficients(cutoff);
                prevCutoff = cutoff;
            }

            for (int channel = 0; channel < 2; ++channel) {
                float* channelData = bufferToWritePointer->getWritePointer(channel);
                const float* rawData = readBuff->getReadPointer(channel);
                float* fftDataChannel = fftData[channel].data();
                float* tempDataChannel = tempData[channel].data();

                float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
                float* overlapBuffer2 = overlapStore[1]->getWritePointer(channel);
                float* overlapBuffer3 = overlapStore[2]->getWritePointer(channel);

                // Shift the previous samples forward
                for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                    fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
                }

                // Append new samples to the end of the fftData buffer
                int numSamplesToProcess = std::min(maxAccumulation, accumulation);
                for (int i = 0; i < numSamplesToProcess; ++i) {
                    fftDataChannel[fftSize - maxAccumulation + i] = rawData[sampleIndex + i];
                }

                // Apply windowing function
                for (int i = 0; i < fftSize; ++i) {
                    tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
                }

                // Perform FFT
                forwardFFT.performRealOnlyForwardTransform(tempDataChannel);

                // Apply filter coefficients
                int half_plus_one = fftSize / 2 + 1;
                for (int i = 0; i < half_plus_one; ++i) {
                    float realPart = tempDataChannel[2 * i];
                    float imagPart = tempDataChannel[2 * i + 1];
                    tempDataChannel[2 * i] = realPart * filterCoefficients[i];
                    tempDataChannel[2 * i + 1] = imagPart * filterCoefficients[half_plus_one + i];
                }

                // Perform inverse FFT
                inverseFFT.performRealOnlyInverseTransform(tempDataChannel);

                // Overlap-add
                for (int i = 0; i < numSamplesToProcess; ++i) {
                    channelData[sampleIndex + i] += (tempDataChannel[i] +
                                                     overlapBuffer3[i + maxAccumulation] +
                                                     overlapBuffer2[i + 2 * maxAccumulation] +
                                                     overlapBuffer1[i + 3 * maxAccumulation]) / 4.0f;
                }

                // Store the new overlap samples
                std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
            }

            // Rotate the overlap buffers
            juce::AudioBuffer<float>* temp = overlapStore[0];
            overlapStore[0] = overlapStore[1];
            overlapStore[1] = overlapStore[2];
            overlapStore[2] = temp;

            sampleIndex += maxAccumulation;
            accumulation -= maxAccumulation;
        }

        // Process any remaining samples less than maxAccumulation
        if (accumulation > 0) {
            for (int channel = 0; channel < 2; ++channel) {
                float* fftDataChannel = fftData[channel].data();
                const float* rawData = readBuff->getReadPointer(channel);

                for (int i = 0; i < accumulation; ++i) {
                    fftDataChannel[fftSize - accumulation + i] = rawData[sampleIndex + i];
                }
            }
        }

    }


    // while processing we calculate the fft for 1024 samples
    // at a time, and we process for every 256 samples.
    //
    // that is we always have a 75% overlap,
    // for 512 samples we need to run this twice for a block
    // and for 64 samples we wait till they get accumulated till
    // 256 samples.
    void processGraphNode() override {
        Generalised();
    }

    void updateFilterCoefficients(float cutoff) {
        float mode = InputSockets[2]->getValue();
        float mix = InputSockets[3]->getValue();

        displayPointer->setValues(cutoff, mode, mix);

        float cutoffFrequency = std::clamp(cutoff * 20000.0f, 20.0f, 20000.0f);

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
            filterCoefficients[half_plus_one + i] = val;

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

        prevCutoff = -1.0f;
    }

    void mini_reset() override {
        updateFilterCoefficients(InputSockets[1]->getValue() / 20000.0f);
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
    juce::AudioBuffer<float> *readBuff = nullptr, partStoreBufferIN, partStoreBufferOUT;
    int partBufferFillIndex = 0;

    int accumulation = 0;

    std::vector<float> windowCoefficients, filterCoefficients;

    const int maxAccumulation = 256;

    juce::AudioBuffer<float>* overlapStore[3] = { nullptr, nullptr , nullptr };

    float prevCutoff = -1.0f;

};

class ResonatingFilter : public GraphNode {
public:

    ResonatingFilter(int posX, int posY) : GraphNode(juce::String("Resonating Filter"), posX, posY), forwardFFT(10),
                                        inverseFFT(10), fftOrder(10), fftSize(1024),
                                        filterCoefficients(fftSize, 0.0f),
                                        partStoreBufferIN(2, 256), partStoreBufferOUT(2, 256) {

        // generate and store the window coefficients.
        windowCoefficients.resize(fftSize);
        juce::dsp::WindowingFunction<float>::fillWindowingTables(windowCoefficients.data(), fftSize, juce::dsp::WindowingFunction<float>::triangular);

        fftData[0].resize(fftSize, 0.0f);
        fftData[1].resize(fftSize, 0.0f);

        tempData[0].resize(2 * fftSize, 0.0f);
        tempData[1].resize(2 * fftSize, 0.0f);

        overlapStore[0] = new juce::AudioBuffer<float>(2, fftSize);
        overlapStore[1] = new juce::AudioBuffer<float>(2, fftSize);
        overlapStore[2] = new juce::AudioBuffer<float>(2, fftSize);

        std::fill(filterCoefficients.begin(), filterCoefficients.end(), 1.0f);

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(120.0, 20000.0, 440.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mode"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-8.0f, 8.0, 3.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mix"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Resonance"), direction::IN, false)); // New Resonance Socket
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(0.1f, 10.0f, 1.0f);

        InputSockets[1]->setSilderCallbackWanted();
        InputSockets[2]->setSilderCallbackWanted();
        InputSockets[3]->setSilderCallbackWanted();
        InputSockets[4]->setSilderCallbackWanted(); // Callback for Resonance

        InputSockets.add(new GraphNode::Socket(juce::String("Transfer Function"), direction::IN, false));
        InputSockets[5]->addFilterDisplayControl(); // Updated Index

        displayPointer = (filterTransferFunctionDisp*)InputSockets[5]->getPointerToPresentParameterCtrlController(); // Updated Index

        needsReloading.store(true);

        makeAllSocketsVisible();

        resized();
    }

    filterTransferFunctionDisp* displayPointer = nullptr;

    void releaseResources() override {}

    void BiggerOrEqualBufferSize() {
        const int numSamples = bufferToWritePointer->getNumSamples();

        accumulation += numSamples;
        int sampleIndex = 0;

        bufferToWritePointer->clear();

        while ((accumulation - maxAccumulation) >= 0) {
            float cutoff = InputSockets[1]->getValue() / 20000.0f;
            float resonance = InputSockets[4]->getValue(); // Read resonance value
            if (cutoff != prevCutoff) {
                updateFilterCoefficients(cutoff, resonance);
                prevCutoff = cutoff;
            }

            for (int channel = 0; channel < 2; ++channel) {
                float* channelData = bufferToWritePointer->getWritePointer(channel);
                const float* rawData = readBuff->getReadPointer(channel);
                float* fftDataChannel = fftData[channel].data();
                float* tempDataChannel = tempData[channel].data();

                float* overlapBuffer1 = overlapStore[0]->getWritePointer(channel);
                float* overlapBuffer2 = overlapStore[1]->getWritePointer(channel);
                float* overlapBuffer3 = overlapStore[2]->getWritePointer(channel);

                // Shift the previous samples forward by maxAccumulation.
                for (int i = 0; i < fftSize - maxAccumulation; ++i) {
                    fftDataChannel[i] = fftDataChannel[i + maxAccumulation];
                }

                // Append maxAccumulation new samples to the end of the fftData buffer.
                for (int i = 0; i < maxAccumulation; ++i) {
                    fftDataChannel[fftSize - maxAccumulation + i] = rawData[sampleIndex + i];
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

                // Overlap-add the processed samples into the output buffer.
                for (int i = 0; i < maxAccumulation; ++i) {
                    channelData[sampleIndex + i] += (   tempDataChannel[i] +
                                                        overlapBuffer3[i + maxAccumulation] +
                                                        overlapBuffer2[i + 2 * maxAccumulation] +
                                                        overlapBuffer1[i + 3 * maxAccumulation]) / 4.0f;
                }

                // Store the new overlap samples
                std::copy(tempDataChannel, tempDataChannel + fftSize, overlapBuffer1);
            }

            // change the order of the pointers as
            // from     [ 1, 2, 3 ]
            // to       [ 2, 3, 1 ]
            juce::AudioBuffer<float>* temp = overlapStore[0];
            overlapStore[0] = overlapStore[1];
            overlapStore[1] = overlapStore[2];
            overlapStore[2] = temp;

            sampleIndex += maxAccumulation;
            accumulation -= maxAccumulation;
        }
    }

    void processGraphNode() override {
        BiggerOrEqualBufferSize();
    }

    void updateFilterCoefficients(float cutoff, float resonance) {
        float mode = InputSockets[2]->getValue();
        float mix = InputSockets[3]->getValue();

        displayPointer->setValues(cutoff, mode, mix);

        float cutoffFrequency = std::clamp(cutoff * 20000.0f, 120.0f, 20000.0f);
        resonance = std::clamp(resonance, 0.1f, 10.0f);

        int half_plus_one = fftSize / 2 + 1;

        for (int i = 0; i < half_plus_one; ++i) {
            float frequency = (float)i * ((float)sampleRate / (float)fftSize);

            float s = frequency / cutoffFrequency;
            float power;

            if (mode >= 0.0f) power = std::pow(s, mode);
            else power = 1.0f / std::pow(s, std::abs(mode));

            float gain = 1.0f / std::sqrt(1.0f + std::pow(s, 4.0f) - 2.0f * std::pow(s, 2.0f) * (1.0f - std::pow(resonance, 2.0f)));
            float val = safeClamp((mix * power * gain + 1.0f) / (power * gain + 1.0f), 0.0f, 1.0f);

            filterCoefficients[i] = val;
            filterCoefficients[half_plus_one + i] = val;
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

        prevCutoff = -1.0f;
    }

    void mini_reset() override {
        updateFilterCoefficients(InputSockets[1]->getValue() / 20000.0f, InputSockets[4]->getValue());
    }

private:

    void (DigitalFilter::*callback_pointer)() = nullptr;

    std::atomic<bool> needsReloading{};

    juce::dsp::FFT forwardFFT, inverseFFT;

    int oo = 1;

    const int fftSize;
    const int fftOrder;

    std::vector<float> fftData[2], tempData[2];

    juce::AudioBuffer<float> *readBuff = nullptr, partStoreBufferIN, partStoreBufferOUT;
    int partBufferFillIndex = 0;

    int accumulation = 0;

    std::vector<float> windowCoefficients, filterCoefficients;

    const int maxAccumulation = 256;

    juce::AudioBuffer<float>* overlapStore[3] = { nullptr, nullptr , nullptr };

    float prevCutoff = -1.0f;

};


//void updateFilterCoefficients(float cutoff) {
//    float mode = InputSockets[2]->getValue();
//    float mix = InputSockets[3]->getValue();
//
//    displayPointer->setValues(cutoff, mode, mix);
//
//    float cutoffFrequency = std::clamp(cutoff * 20000.0f, 120.0f, 20000.0f);
//
//    int half_plus_one = fftSize / 2 + 1;
//
//    for (int i = 0; i < half_plus_one; ++i) {
//        float frequency = (float)i * ((float)sampleRate / (float)fftSize);
//
//        float s = frequency / cutoffFrequency;
//        float power;
//
//        if (mode >= 0.0f) power = std::pow(s , mode);
//        else power = 1.0f / std::pow(s , std::abs(mode));
//
//        float val = safeClamp(( mix * power + 1.0f ) / ( power + 1.0f ), 0.0f, 1.0f);
//
//        float phaseDelay;
//
//        if (mode >= 0.0f) { phaseDelay = -(std::atan(power))/2.0f; }
//        else { phaseDelay = ( PI_HALF - (std::atan(power))) / 2.0f; }
//
//        filterCoefficients[i] = val * std::cos(phaseDelay);
//        filterCoefficients[half_plus_one + i] = val * std::sin(phaseDelay);
//
//    }
//
//}



//    // when the buffer size < 256, we store the results in another buffer which we
//    // will be using parts of the processed data to fill the bufferToWrite.
//    void SmallerBufferSize() {
//        const int numSamples = bufferToWritePointer->getNumSamples();
//
//        accumulation += numSamples;
//        int sampleIndex = 0;
//
//        for (int ch = 0; ch < 2; ++ch) {
//            float* channelData = partStoreBufferOUT.getWritePointer(ch);
//            const float* rawData = readBuff->getReadPointer(ch);
//
//            for (int i = partBufferFillIndex, j = 0; i < partBufferFillIndex+numSamples; ++i, ++j) {
//                channelData[i] = rawData[j];
//            }
//
//        }
//
//        while ((accumulation - maxAccumulation) >= 0) {
//            if (needsReloading.load()) {
//                updateFilterCoefficients();
//                needsReloading.store(false);
//            }
//
//            for (int channel = 0; channel < 2; ++channel) {
//                float* channelData = partStoreBufferOUT.getWritePointer(channel);
//                const float* rawData = partStoreBufferIN.getReadPointer(channel);
//                float* fftDataChannel = fftData[channel].data();
//                float* tempDataChannel = tempData[channel].data();
//
//
//                // Shift the previous samples forward by maxAccumulation.
//                for (int i = fftSize-1; i >= maxAccumulation; --i) {
//                    fftDataChannel[i] = fftDataChannel[i - maxAccumulation];
//                }
//                // Append maxAccumulation new samples to a 1024 sized buffer making it a 75% overlap.
//                for (int i = sampleIndex, j = 0; j < maxAccumulation; ++i, ++j) {
//                    fftDataChannel[j] = rawData[i];
//                }
//
//                // Write this to the tempDataChannel with the windowing function applied.
//                for (int i = 0; i < fftSize; ++i) {
//                    tempDataChannel[i] = fftDataChannel[i] * windowCoefficients[i];
//                }
//
//                // Perform an inplace FFT on the tempDataChannel.
//                forwardFFT.performRealOnlyForwardTransform(tempDataChannel);
//
//                // Multiply with the corresponding filter coefficients.
//                for (int i = 0; i < fftSize / 2 + 1; ++i) {
//                    float realPart = tempDataChannel[2 * i];
//                    float imagPart = tempDataChannel[2 * i + 1];
//                    float filterCoeff = filterCoefficients[i];
//                    tempDataChannel[2 * i] = realPart * filterCoeff;
//                    tempDataChannel[2 * i + 1] = imagPart * filterCoeff;
//                }
//
//                // Perform the inverse FFT
//                inverseFFT.performRealOnlyInverseTransform(tempDataChannel);
//
//                // Add the processed data back to the output buffer
//                for (int i = 0, j = sampleIndex; i < maxAccumulation; ++i, ++j) {
//                    channelData[j] = tempDataChannel[i] / static_cast<float>(fftSize);
//                }
//
//            }
//
//            partBufferFillIndex = 0;
//            accumulation = 0;
//        }
//
//        // copy the present buffer size amount of samples.
//        for (int ch = 0; ch < 2; ++ch) {
//            float* channelData = bufferToWritePointer->getWritePointer(ch);
//            const float* rawData = partStoreBufferOUT.getReadPointer(ch);
//
//            for (int i = partBufferFillIndex, j = 0; i < partBufferFillIndex+numSamples; ++i, ++j) {
//                channelData[j] = rawData[i];
//            }
//
//        }
//
//        partBufferFillIndex += numSamples;
//
//    }