/*
  ==============================================================================

    Absolute.h
    Created: 23 May 2024 3:42:33am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Absolute : public GraphNode {
public:

    Absolute(int pos_x, int pos_y) : GraphNode(juce::String("Absolute"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        makeAllSocketsVisible();

        resized();
    }

    void processGraphNode() override {
        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                writePointer[i] = std::abs(signal_1[i]);
                writePointer[i+1] = std::abs(signal_1[i+1]);
            }
        }
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        BufferOne = InputSockets[0]->getBufferPointer();
    }

    ~Absolute() {};

private:

    juce::AudioBuffer<float>* BufferOne;

};
