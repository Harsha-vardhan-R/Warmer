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

    GraphNode* last;

    OutputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("AUDIO OUT",
                      pos_x,
                      pos_y,
                      120,
                      160) {

        last = nullptr;

    };

    ~OutputMasterGraphNode() override {};

    bool isConnected() {
        return (last != nullptr);
    }

    void process() override {};


};