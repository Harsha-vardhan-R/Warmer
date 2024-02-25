/*
  ==============================================================================

    DemoNode.h
    Created: 24 Feb 2024 7:50:35pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"

class DemoNode : public GraphNode {
public :

    DemoNode(int pos_x, int pos_y);
    ~DemoNode() override {};

};