/*
  ==============================================================================

    InputMasterGraphNode.h
    Created: 4 Mar 2024 9:59:02pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"
#include "Socket.h"
#include "InputOutputTypesForSokets.h"

class InputMasterGraphNode : public GraphNode {
public:

    InputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("MIDI IN",
                      pos_x,
                      pos_y) {

        OutputSockets.add(new Socket(juce::String("MIDI"), direction::OUT, false));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        OutputSockets.add(new Socket(juce::String("Pitch Wheel"), direction::OUT, false));
        OutputSockets[1]->setOutputType(SocketDataType::FloatingVector);

        OutputSockets.add(new Socket(juce::String("Mod Wheel"), direction::OUT, false));
        OutputSockets[2]->setOutputType(SocketDataType::FloatingVector);

        canBeDeleted = false;
        resized();
    };

    void process() override {};

    void update(double bitRate, int bufferSize) override {}

    ~InputMasterGraphNode() override {};

};