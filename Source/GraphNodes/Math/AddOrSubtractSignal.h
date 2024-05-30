/*
  ==============================================================================

    AddOrSubtractSignal.h
    Created: 11 May 2024 6:06:57pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"


// add a fraction of a signal to another signal.
// if the value of fraction is 1 it is the same as adding,
// for -1 you basically subtract.
class AddOrSubtract : public GraphNode {
public:

    AddOrSubtract(int pos_x, int pos_y) : GraphNode(juce::String("Add Or Subtract"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal 1"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal 2"), direction::IN, true));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("1-2 <- 1 -> 1+2"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(-1.0, 1.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModulation(float fraction) {

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            const float* signal_2 = BufferTwo->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                writePointer[i] = signal_1[i] + (signal_2[i] * fraction);
                writePointer[i+1] = signal_1[i+1] + (signal_2[i+1] * fraction);
            }
        }

    }

    void yesModulation(float fraction) {

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal_1 = BufferOne->getReadPointer(channel);
            const float* signal_2 = BufferTwo->getReadPointer(channel);
            const float* modBuffChannel = ModulatingBuffer->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                writePointer[i] = signal_1[i] + (signal_2[i] * modBuffChannel[i]);
                writePointer[i+1] = signal_1[i+1] + (signal_2[i+1] * modBuffChannel[i+1]);
            }
        }

    }

    void processGraphNode() override {
        float val = InputSockets[2]->getValue();

        (this->*callbackFunction)(val);
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        ModulatingBuffer = InputSockets[2]->getBufferPointer();

        BufferOne = InputSockets[0]->getBufferPointer();
        BufferTwo = InputSockets[1]->getBufferPointer();

        if (InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            callbackFunction = &AddOrSubtract::yesModulation;
        } else {
            callbackFunction = &AddOrSubtract::noModulation;
        }
    }

private:

    juce::AudioBuffer<float>* BufferOne, *BufferTwo;

    juce::AudioBuffer<float>* ModulatingBuffer = nullptr;

    void (AddOrSubtract::*callbackFunction)(float) = nullptr;

};
