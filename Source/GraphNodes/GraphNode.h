/*
  ==============================================================================

    GraphNode.h
    Created: 22 Feb 2024 4:51:24pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

// Class that all the nodes inherit.
class GraphNode : public juce::Component {
public :

    // Height of the node Component.
    // This is not the rendering height, it will depend on the zoom level.
    int UIWidth = 20;
    int UIHeight = 40;

    //Location in the 2d graph.
    virtual int UI_X;
    virtual int UI_Y;

    // need to give the proportions for the node.
    virtual GraphNode(int x = 20, int y = 40) {
        UI_X = x;
        UI_Y = y;

        resized()
    };
    virtual ~GraphNode() override {};

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::colours::black);
        g.setcolour(juce::Colours::white);
        g.drawText("Demo Look", g.getClipBounds(), juce::Justification::centred);
    };
    void resized() override {
        setBounds(0, 0, )
    };
};