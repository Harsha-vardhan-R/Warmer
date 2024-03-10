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
                      pos_y,
                      120,
                      160) {

    };

    ~OutputMasterGraphNode() override {};

    void paint(juce::Graphics& g) override {
        paintBasic(g, 0);
    }

};