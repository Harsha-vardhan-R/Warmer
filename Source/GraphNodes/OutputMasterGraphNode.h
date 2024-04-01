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
                      pos_y) {

        InputSockets.add(new Socket(juce::String("Audio Out"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        canBeDeleted = false;
        makeAllSocketsVisible();
        resized();
    };

    ~OutputMasterGraphNode() {};

    bool isConnected() { return (last != nullptr); }

    void update(double bitRate, int bufferSize) override {}

    void process() override {};


};