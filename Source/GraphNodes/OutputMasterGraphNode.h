/*
  ==============================================================================

    OutputMasterGraphNode.h
    Created: 4 Mar 2024 9:59:22pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"

class OutputMasterGraphNode : public GraphNode {
public:

    OutputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("AUDIO OUT",
                      pos_x,
                      pos_y) {

        needAudioBuffer = false;

        InputSockets.add(new Socket(juce::String("Audio Out"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        canBeDeleted = false;

        makeAllSocketsVisible();

        resized();
    };

    void processGraphNodeOutput(float* const* outputChannelData, float volumeLevel) {

        // just copy the data with the given volume level.
        const float* channelOneData = readBuff->getReadPointer(0);
        const float* channelTwoData = readBuff->getReadPointer(1);

        float* outputChannelOneData = outputChannelData[0];
        float* outputChannelTwoData = outputChannelData[1];

        for (int i = 0; i < estimatedSamplesPerBlock; ++i) {
            outputChannelOneData[i] = channelOneData[i]*volumeLevel;
        }

        for (int i = 0; i < estimatedSamplesPerBlock; ++i) {
            outputChannelTwoData[i] = channelTwoData[i]*volumeLevel;
        }

    }

    ~OutputMasterGraphNode() {};

    bool allGood() { return (InputSockets[0]->isThisConnected()); }

    // this will not be called.
    void processGraphNode() override {}

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();
    }

private:
    juce::AudioBuffer<float>* readBuff;

};