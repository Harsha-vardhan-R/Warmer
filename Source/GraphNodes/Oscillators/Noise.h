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
        InputSockets[1]->addSliderParameterControl(0.0, 1.2, 0.2);


        makeAllSocketsVisible();
        resized();
    }

    void generateWhiteNoise(float* outputChannel, float l) {
        // Generate white noise samples and fill the channel.
        for (int i = 0; i < estimatedSamplesPerBlock; ++i) outputChannel[i] = dis(gen)*l;
    }


    // pink noise relates the Amplitude to the frequency as a function nearly matching (1/(f^c)),
    // where f is the frequency and the c is some constant number between ~0 and ~3,
    // although this formula is not enough to generate the noise.
    void generatePinkNoise(float* outputChannel, float l) {
        // Create a random number generator


    }

    void generateBrownNoise(float* outputChannel, float l) {
        // Create a random number generator



    }

    void generateBlueNoise(float* outputChannel, float l) {
        // Create a random number generator



    }

    void processGraphNode() override {
        int val = (int)InputSockets[0]->getValue();
        float gain = InputSockets[1]->getValue();

        generateWhiteNoise(bufferToWritePointer->getWritePointer(0), gain);
        generateWhiteNoise(bufferToWritePointer->getWritePointer(1), gain);
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
    }

    ~Noise() {};

private:

    // random device and the generator initializations.
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

};