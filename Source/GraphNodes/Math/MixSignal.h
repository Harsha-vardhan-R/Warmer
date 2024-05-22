/*
  ==============================================================================

    MixSignal.h
    Created: 11 May 2024 6:02:20pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class MixSignal : public GraphNode {
public:

    MixSignal(int pos_x, int pos_y) : GraphNode(juce::String("Mix"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal 1"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal 2"), direction::IN, true));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Amount 1<-1|2->2"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-1.0, 1.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModulation() {

        // -1 for only 1, 0 for (0.5*signal_1) + (0.5*signal_2)
        float ratio = InputSockets[2]->getValue();

        float sig_1_amount = (ratio * -0.5) + 0.5;
        float sig_2_amount = (ratio * 0.5) + 0.5;

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            const float* signal_2 = BufferTwo->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                writePointer[i] = (signal_1[i] * sig_1_amount) + (signal_2[i] * sig_2_amount);
                writePointer[i+1] = (signal_1[i+1] * sig_1_amount) + (signal_2[i+1] * sig_2_amount);
            }
        }

    }

    void yesModulation() {

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            const float* signal_2 = BufferTwo->getReadPointer(channel);
            const float* modBuffChannel = ModulatingBuffer->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                float ratio = modBuffChannel[i];
                float sig_1_amount = -(ratio/2) + 0.5;
                float sig_2_amount = (ratio/2) + 0.5;
                writePointer[i] = (signal_1[i] * sig_1_amount) + (signal_2[i] * sig_2_amount);
            }
        }

    }

    void processGraphNode() override {
        (this->*callbackFunction)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        ModulatingBuffer = InputSockets[2]->getBufferPointer();

        BufferOne = InputSockets[0]->getBufferPointer();
        BufferTwo = InputSockets[1]->getBufferPointer();

        if (InputSockets[2]->isThisConnected()) {
            callbackFunction = &MixSignal::yesModulation;
        } else {
            callbackFunction = &MixSignal::noModulation;
        }
    }

    ~MixSignal() {};

private:

    juce::AudioBuffer<float>* BufferOne, *BufferTwo;

    juce::AudioBuffer<float>* ModulatingBuffer = nullptr;

    void (MixSignal::*callbackFunction)() = nullptr;

};
