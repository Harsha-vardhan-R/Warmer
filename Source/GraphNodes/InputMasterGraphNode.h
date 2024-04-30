/*
  ==============================================================================

    InputMasterGraphNode.h
    Created: 4 Mar 2024 9:59:02pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"

class InputMasterGraphNode : public GraphNode {
public:

    InputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("MIDI IN",
                      pos_x,
                      pos_y) {

        OutputSockets.add(new Socket(juce::String("MIDI"), direction::OUT, false));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        OutputSockets.add(new Socket(juce::String("Pitch Wheel"), direction::OUT, false));
        OutputSockets[1]->setOutputType(SocketDataType::AudioBufferFloatSingle);

        OutputSockets.add(new Socket(juce::String("Mod Wheel"), direction::OUT, false));
        OutputSockets[2]->setOutputType(SocketDataType::AudioBufferFloatSingle);

        canBeDeleted = false;
        makeAllSocketsVisible();
        resized();
    };

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {}


    void releaseResources() override {}
    void reset() override {}

    ~InputMasterGraphNode() {};

};