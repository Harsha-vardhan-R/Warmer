/*
  ==============================================================================

    Convolution.h
    Created: 12 May 2024 11:26:13am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

class Convolution : public GraphNode {
public:

    Convolution(int pos_x, int pos_y)  :  GraphNode(juce::String("Convolution"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        // Amplitude.
        InputSockets.add(new GraphNode::Socket(juce::String("This sample"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(-1.0, 1.0, 1.0);

        InputSockets.add(new GraphNode::Socket(juce::String("prev."), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-1.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("prev.."), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(-1.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("prev..."), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(-1.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("prev...."), direction::IN, false));
        InputSockets[5]->acceptType(SocketDataType::Floating);
        InputSockets[5]->addSliderParameterControl(-1.0, 1.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }


    void processGraphNode() override {
        float this_comp = InputSockets[1]->getValue();
        float prev_comp = InputSockets[2]->getValue();
        float prev_comp_two = InputSockets[3]->getValue();
        float prev_comp_three = InputSockets[4]->getValue();
        float prev_comp_four = InputSockets[5]->getValue();

        int size_temp = bufferToWritePointer->getNumSamples();

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputData = readBuff->getReadPointer(channel);

            // set based on prev samples
            channelData[0] = inputData[0]*this_comp + store_from_prev[channel][3]*prev_comp + store_from_prev[channel][2]*prev_comp_two + store_from_prev[channel][1]*prev_comp_three + store_from_prev[channel][1]*prev_comp_four;
            channelData[1] = inputData[1]*this_comp + inputData[0]*prev_comp + store_from_prev[channel][3]*prev_comp_two + store_from_prev[channel][2]*prev_comp_three + store_from_prev[channel][1]*prev_comp_four;
            channelData[2] = inputData[2]*this_comp + inputData[1]*prev_comp + inputData[0]*prev_comp_two + store_from_prev[channel][3]*prev_comp_three + store_from_prev[channel][2]*prev_comp_four;
            channelData[3] = inputData[3]*this_comp + inputData[2]*prev_comp + inputData[1]*prev_comp_two + inputData[0]*prev_comp_three + store_from_prev[channel][3]*prev_comp_four;

            for (int i = 4; i < size_temp; ++i) channelData[i] = inputData[i]*this_comp + inputData[i-1]*prev_comp + inputData[i-2]*prev_comp_two + inputData[i-3]*prev_comp_three * inputData[i-4]*prev_comp_four;

            store_from_prev[channel][3] = inputData[size_temp-1];
            store_from_prev[channel][2] = inputData[size_temp-2];
            store_from_prev[channel][1] = inputData[size_temp-3];
            store_from_prev[channel][0] = inputData[size_temp-4];

        }
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        readBuff = InputSockets[0]->getBufferPointer();
    }

    void mini_reset() {}

    ~Convolution() {};

private:

    juce::AudioBuffer<float>* readBuff;

    float store_from_prev[2][4] = {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};

};