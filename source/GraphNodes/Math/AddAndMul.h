/*
  ==============================================================================

    AddAndMul.h
    Created: 11 May 2024 6:02:11pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

// This actually does multiplication then add unlike what the name says.
class AddAndMul : public GraphNode {
public:

    AddAndMul(int pos_x, int pos_y) : GraphNode(juce::String("Multiply & Add"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Mul"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(-10000.0, 10000.0, 1.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Add"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(-10000.0, 10000.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModulation() {

        float multiply_with = InputSockets[1]->getValue();
        float add_val = InputSockets[2]->getValue();

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal = readBuff->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                writePointer[i] = (signal[i] * multiply_with) + add_val;
            }
        }

    }

    void onlyAddModulating() {
        float multiply_with = InputSockets[1]->getValue();

        for ( int channel = 0; channel < 2; channel++) {
            const float* signal = readBuff->getReadPointer(channel);
            const float* add_val = ModulatingBuffer_add->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                writePointer[i] = (signal[i] * multiply_with) + add_val[i];
            }
        }
    }

    void onlyMulModulating() {

        float add_val = InputSockets[2]->getValue();

        for ( int channel = 0; channel < 2; channel++) {
            const float* multiply_with = ModulatingBuffer_mul->getReadPointer(channel);
            const float* signal = readBuff->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                writePointer[i] = (signal[i] * multiply_with[i]) + add_val;
            }
        }

    }

    void bothModulating() {
        for ( int channel = 0; channel < 2; channel++) {
            const float* add_val = ModulatingBuffer_add->getReadPointer(channel);
            const float* multiply_with = ModulatingBuffer_mul->getReadPointer(channel);
            const float* signal = readBuff->getReadPointer(channel);
            float* writePointer = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                writePointer[i] = (signal[i] * multiply_with[i]) + add_val[i];
            }
        }
    }

    void processGraphNode() override {
        (this->*callbackFunction)();
    }

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        ModulatingBuffer_add = InputSockets[2]->getBufferPointer();
        ModulatingBuffer_mul = InputSockets[1]->getBufferPointer();

        bool add_mod = InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat;
        bool mul_mod = InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat;

        if (add_mod && mul_mod) {
            callbackFunction = &AddAndMul::bothModulating;
        } else if (add_mod) {
            callbackFunction = &AddAndMul::onlyAddModulating;
        } else if (mul_mod) {
            callbackFunction = &AddAndMul::onlyMulModulating;
        } else {
            callbackFunction = &AddAndMul::noModulation;
        }
    }

    ~AddAndMul() {};

private:

    juce::AudioBuffer<float>* readBuff;

    juce::AudioBuffer<float>* ModulatingBuffer_add = nullptr;
    juce::AudioBuffer<float>* ModulatingBuffer_mul = nullptr;

    void (AddAndMul::*callbackFunction)() = nullptr;

};
