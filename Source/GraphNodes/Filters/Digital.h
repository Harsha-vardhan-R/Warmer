/*
  ==============================================================================

    Digital.h
    Created: 12 May 2024 11:29:06am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"


class DigitalFilter : public GraphNode {
public:

    Filter(int pos_x, int pos_y) : GraphNode(juce::String("Filter"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(0.0f, 4000.0f, 0.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("Resonance"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0f, 1.0f, 0.2f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mix(Dry<->Wet)"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0f, 1.0f, 0.3f);

        makeAllSocketsVisible();

        resized();
    }


    void processGraphNode() override {

        int numSamples = bufferToWritePointer.getNumSamples();

        for (int channel = 0; channel < 2; channel++) {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex += fftSize) {
                int copySize = std::min(fftSize, numSamples - sampleIndex);

                // Copy and window the samples
                windowedBuffer.clear();
                for (int i = 0; i < copySize; ++i) {
                    windowedBuffer.setSample(0, i, channelData[sampleIndex + i] * window[i]);
                }

                // Perform FFT
                fft->performRealOnlyForwardTransform(windowedBuffer.getWritePointer(0));

                // Apply frequency domain processing here (e.g., filtering)
                applyFrequencyResponse(windowedBuffer, cutoffFrequency);

                // Prepare full complex array for IFFT
                prepareComplexArrayForIFFT();

                // Perform inverse FFT
                fft->performRealOnlyInverseTransform(fftBuffer.getWritePointer(0));

                // Overlap-add the processed buffer back to the output
                for (int i = 0; i < copySize; ++i) {
                    channelData[sampleIndex + i] += fftBuffer.getSample(0, i);
                }
            }
        }

    }

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
    }

    ~Filter() {};

private:

    // fft with the fft order as 10,
    // i.e we are going to consider 1024 samples.
    juce::dsp::FFT fftObject(10);

    juce::AudioBuffer<float>* readBuff;


};