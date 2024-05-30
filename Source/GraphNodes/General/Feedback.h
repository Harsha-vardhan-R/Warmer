//
// Created by harshavardhan on 30/5/24.
//

#pragma once
#include "../GraphNode.h"

class Feedback : public GraphNode {
public:

    Feedback(int pos_x, int pos_y) : GraphNode(juce::String("Feedback"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Feed IN"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Take OUT"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Amount"), direction::IN, false));
        InputSockets[1]->addSliderParameterControl(0.0, 1.0f, 0.5);
        InputSockets[1]->acceptType(SocketDataType::Floating);

        doNotRecycleBuffer();

        noRearrangeAtConnection();

        makeAllSocketsVisible();

        resized();
    }

    void processGraphNode() override {
        float amount = InputSockets[1]->getValue();

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                writePointer[i] = signal_1[i]*amount;
                writePointer[i+1] = signal_1[i+1]*amount;
            }
        }
    }

    void releaseResources() override {}

    void reset() override {
        BufferOne = InputSockets[0]->getBufferPointer();
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
    }

    ~Feedback() {};

private:

    juce::AudioBuffer<float>* BufferOne = nullptr;

};
