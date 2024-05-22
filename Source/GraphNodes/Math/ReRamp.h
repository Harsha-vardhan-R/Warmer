/*
  ==============================================================================

    ReRamp.h
    Created: 11 May 2024 11:53:03pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

// Applies a linear re-mapping of a signal from 0..1 to low.high,
// if the low is greater than high the signal gets inverted but may not lie in the same
// range.
class ReRamp : public GraphNode {
public:

    ReRamp(int pos_x, int pos_y) : GraphNode(juce::String("Re-map"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Low"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 100.0, 1.0);

        InputSockets.add(new GraphNode::Socket(juce::String("High"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(0.0, 100.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModulation() {
        float low = InputSockets[1]->getValue();
        float high = InputSockets[2]->getValue();

        float slope = -(low-high);

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = slope*inputSignal[i] + low;
                channelData[i+1] = slope*inputSignal[i+1] + low;
            }
        }
    }

    void onlyLowModulating() {
        float high = InputSockets[2]->getValue();

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);
            const float* low_mod = ModulatingBuffer_low->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = (-(low_mod[i] - high)*inputSignal[i]) + low_mod[i];
                channelData[i+1] = (-(low_mod[i+1] - high)*inputSignal[i+1]) + low_mod[i+1];
            }
        }
    }

    void onlyHighModulating() {
        float low = InputSockets[1]->getValue();

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);
            const float* high_mod = ModulatingBuffer_high->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = (-(low - high_mod[i])*inputSignal[i]) + low;
                channelData[i+1] = (-(low - high_mod[i+1])*inputSignal[i+1]) + low;
            }
        }
    }

    void bothModulating() {
        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);
            const float* low_mod = ModulatingBuffer_low->getReadPointer(channel);
            const float* high_mod = ModulatingBuffer_high->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = (-(low_mod[i] - high_mod[i])*inputSignal[i]) + low_mod[i];
                channelData[i+1] = (-(low_mod[i+1] - high_mod[i+1])*inputSignal[i+1]) + low_mod[i+1];
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

        ModulatingBuffer_high = InputSockets[1]->getBufferPointer();
        ModulatingBuffer_low = InputSockets[2]->getBufferPointer();

        bool high_mod = InputSockets[1]->isThisConnected();
        bool low_mod = InputSockets[2]->isThisConnected();

        if (high_mod && low_mod) {
            callbackFunction = &ReRamp::bothModulating;
        } else if (low_mod) {
            callbackFunction = &ReRamp::onlyLowModulating;
        } else if (high_mod) {
            callbackFunction = &ReRamp::onlyHighModulating;
        } else {
            callbackFunction = &ReRamp::noModulation;
        }
    }


private:

    juce::AudioBuffer<float>* readBuff;

    juce::AudioBuffer<float>* ModulatingBuffer_low = nullptr;
    juce::AudioBuffer<float>* ModulatingBuffer_high = nullptr;

    void (ReRamp::*callbackFunction)() = nullptr;

};