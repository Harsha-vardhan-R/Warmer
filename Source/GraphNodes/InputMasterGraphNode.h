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
                      pos_y,
                      60,
                      80) {

        OUTSocket[0] = SocketDataType::MIDI;
        OUTSocket[1] = SocketDataType::Floating;
        OUTSocket[2] = SocketDataType::Floating;

        OutputSocketNames[0] = juce::String("MIDI Data");
        OutputSocketNames[1] = juce::String("Mod Wheel Value");
        OutputSocketNames[2] = juce::String("Pitch Wheel Value");
    };
    ~InputMasterGraphNode() override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputMasterGraphNode)
};