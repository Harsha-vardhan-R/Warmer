/*
  ==============================================================================

    Digital.h
    Created: 12 May 2024 11:29:06am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

//
//class DigitalFilter : public GraphNode {
//public:
//
//    DigitalFilter(int posX, int posY) : GraphNode(juce::String("Filter Emulate"), posX, posY), forwardFFT(10),
//                                                                inverseFFT(10), fftOrder(10), fftSize(1024) {
//        window.reset(new juce::dsp::WindowingFunction<float>(fftSize, juce::dsp::WindowingFunction<float>::hann));
//        fftData.resize(2 * fftSize, 0.0f);
//        tempData.resize(fftSize, 0.0f);
//    }
//
//    void processGraphNode() override {
//
//        const int numSamples = buffer.getNumSamples();
//        const int numChannels = buffer.getNumChannels();
//
//        for (int channel = 0; channel < numChannels; ++channel) {
//
//            for (int i = 0; i < numSamples; ++i) fftData[i] = buffer.getReadPointer(channel)[i] * window->getWindowingTable()[i];
//
//            // Zero-pad the rest of the FFT buffer if necessary
//            for (int i = numSamples; i < fftSize; ++i) fftData[i] = 0.0f;
//
//            // Perform forward FFT
//            forwardFFT.performRealOnlyForwardTransform(fftData.data());
//
//            // Apply the filter in the frequency domain
//            for (int i = 0; i < filterCoefficients.size(); ++i) {
//                fftData[2 * i] *= filterCoefficients[i];
//                fftData[2 * i + 1] *= filterCoefficients[i];
//            }
//
//            // Perform inverse FFT
//            inverseFFT.performRealOnlyInverseTransform(fftData.data());
//
//            // Copy samples back to buffer
//            for (int i = 0; i < numSamples; ++i)
//                buffer.getWritePointer(channel)[i] = fftData[i] / static_cast<float>(fftSize);
//
//        }
//    }
//
//    void reset() override {
//
//    }
//
//private:
//
//    juce::dsp::FFT forwardFFT, inverseFFT;
//    int fftSize;
//    int fftOrder;
//    std::vector<float> fftData, tempData;
//    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
//    std::array<float, 513> filterCoefficients;
//
//};