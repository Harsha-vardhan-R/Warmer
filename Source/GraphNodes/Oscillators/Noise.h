/*
  ==============================================================================

    Noise.h
    Created: 2 May 2024 10:36:20pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

#define PI 3.14159265358979323846


class Noise : public GraphNode {
public:

    Noise(int pos_x, int pos_y)  :  GraphNode(juce::String("Noise"), pos_x, pos_y),
                                    gen(rd()),
                                    dis(-1.0f, 1.0f) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        // Type of the basic wave shape.
        InputSockets.add(new Socket(juce::String("Noise Type"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("White");
        InputSockets[0]->addMenuItem("Pink");
        InputSockets[0]->addMenuItem("Brown");
        InputSockets[0]->addMenuItem("Blue");

        // Amplitude.
        InputSockets.add(new Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 1.0, 0.1);


        makeAllSocketsVisible();
        resized();
    }

    // functions generate for each channel.
    inline void generateWhiteNoise(juce::AudioBuffer<float>* outputData, float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = outputData->getWritePointer(channel);

            for (int i = 0; i < outputData->getNumSamples(); i += 2) {
                // Generate two samples at a time
                channelData[i] = random.nextFloat() * 2.0f - 1.0f;
                channelData[i + 1] = random.nextFloat() * 2.0f - 1.0f;

                channelData[i] *= l;
                channelData[i + 1] *= l;
            }
        }
    }


    // pink noise relates the Amplitude to the frequency as a function nearly matching (1/(f^c)),
    // where f is the frequency and the c is some constant number between ~0 and ~3,
    // although this formula is not enough to generate the noise.
    // (copied the above 3 lines from a yt video :) )
    inline void generatePinkNoise(juce::AudioBuffer<float>* outputData, float l) noexcept {

        float a = 10*l;

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = outputData->getWritePointer(channel);

            for (int i = 0; i < outputData->getNumSamples(); i += 2) {
                float white1 = dis(gen);
                float white2 = dis(gen);

                // Generate two samples at a time
                channelData[i] = ( white1 * pinkParamTwo) + (prevSample1 * pinkParamOne);
                channelData[i + 1] = ( white2 * pinkParamTwo) + (prevSample2 * pinkParamOne);

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= a;
                channelData[i + 1] *= a;

//                std::cout << channelData[i] << " " << channelData[i+1] << " ";

            }
        }
    }

    inline void generateBrownNoise(juce::AudioBuffer<float>* outputData, float l) noexcept {
        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = outputData->getWritePointer(channel);

            for (int i = 0; i < outputData->getNumSamples(); ++i) {
                float white = dis(gen);
                channelData[i] = prevSample + (0.02f * white);
                prevSample = channelData[i];
                channelData[i] *= l;
            }
        }
    }

    inline void generateBlueNoise(juce::AudioBuffer<float>* outputData, float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = outputData->getWritePointer(channel);

            for (int i = 0; i < outputData->getNumSamples(); i += 2) {
                float white1 = dis(gen);
                float white2 = dis(gen);

                // Generate two samples at a time
                channelData[i] = (0.01f * prevSample1) + white1;
                channelData[i + 1] = (0.01f * prevSample2) + white2;

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= l;
                channelData[i + 1] *= l;

            }
        }
    }

    void processGraphNode() override {
        int val = (int)InputSockets[0]->getValue();
        float gain = InputSockets[1]->getValue();

        switch (val) {
            case 1:
                generateWhiteNoise(bufferToWritePointer, gain);
                break;
            case 2:
                generatePinkNoise(bufferToWritePointer, gain);
                break;
            case 3:
                generateBrownNoise(bufferToWritePointer, gain);
                break;
            case 4:
                generateBlueNoise(bufferToWritePointer, gain);
                break;
        }

        return;

    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        pinkParamOne = std::exp((-2.0 * PI * 30.0) / sampleRate);
        pinkParamTwo = std::sqrt( 1.0 - (pinkParamOne * pinkParamOne) );

        prevSample1 = 0.0f;
        prevSample2 = 0.0f;

        prevSample = 0.0f;
    }

    ~Noise() {};

private:

    // random device and the generator initializations.
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    juce::Random random;

    float pinkParamOne = 0.0;
    float pinkParamTwo = 0.0;

    float prevSample1 = 0.0f;
    float prevSample2 = 0.0f;

    float prevSample = 0.0f;
};