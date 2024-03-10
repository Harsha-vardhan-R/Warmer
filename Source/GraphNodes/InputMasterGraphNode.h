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
                      pos_y,
                      120,
                      350) {

        InputSockets.add(new Socket("Audio IN",
                                    direction::IN));

    };

    ~InputMasterGraphNode() override {};

    void paint(juce::Graphics& g) override {
        paintBasic(g, 0);
    }

};