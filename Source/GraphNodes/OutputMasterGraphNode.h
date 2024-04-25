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

        InputSockets.add(new Socket(juce::String("Audio Out"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        canBeDeleted = false;

        makeAllSocketsVisible();

        resized();
    };

    ~OutputMasterGraphNode() {};

    bool isConnected() { return (InputSockets[0]->isThisConnected()); }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {}
    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override {}
    void releaseResources() override {}
    void reset() override {}

};