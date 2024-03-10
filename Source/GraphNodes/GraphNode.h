/*
  ==============================================================================

    GraphNode.h
    Created: 22 Feb 2024 4:51:24pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>
#include "../ColourPalette.h"
#include "InputOutputTypesForSokets.h" // define the input and output types.
#include "Socket.h"


// Class that all the nodes inherit.

class GraphNode : public juce::Component {
public :


    int zoomLevel;
    juce::OwnedArray<Socket> InputSockets;
    juce::OwnedArray<Socket> OutputSockets;

    // Height of the node Component.
    // This is not the rendering height, it will depend on the zoom level.
    // It is also the dimensions of the node when the zoom level is 0.
    int UIWidth = 20;
    int UIHeight = 40;

    //Location in the 2d graph.
    int UI_X;
    int UI_Y;

    // Name of the node(shown at the top)
    juce::String name;

    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y, int x, int y) {
        UI_X = pos_x;
        UI_Y = pos_y;

        UIWidth = x;
        UIHeight = y;

        this->name = name;
        resized();
    }

    void setZoomLevel(int level) {
        zoomLevel = level;
    }


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g ,int zoomLevel) {
        auto bounds = g.getClipBounds();
        g.setColour(GraphNodeBackgroundColourID);
        g.fillRoundedRectangle(bounds.toFloat().reduced(2), 3.0f);
        g.setColour(GraphNodeHeadingTextID);
        g.drawRoundedRectangle(bounds.toFloat().reduced(2), 3.0f, 1.0f);

        g.setColour(GraphNodeHeadingTextID);
        g.drawText(name, bounds, juce::Justification::centredTop);

        juce::Rectangle<int> area;

//        int index = 0;
//        for (auto i : InputSockets) {
////            area
//            g.drawText(i.name,  juce::Justification::centred)
//            i.bound(2, index*(0.2+(i*0.1)), 2);
//            i++;
//        }
//
//        index = 0;
//        for (auto i : OutputSockets) {
//            i.bound(2, index*(0.2+(i*0.1)), 2);
//            i++;
//        }
    }

    virtual ~GraphNode() override {}

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
    }

    void resized() override {
        setBounds(UI_X, UI_Y, UIWidth, UIHeight);

        // Setting the bounds of Sockets, they will draw themselves as they are Components.
        // we will set it everytime
        int index = 0;
        for (auto i : InputSockets) {
            i->bound(2, getWidth()*(0.2+(index*0.1)), 2);
            index++;
        }

        index = 0;
        for (auto i : OutputSockets) {
            i->bound(2, getWidth()*(0.2+(index*0.1)), 2);
            index++;
        }
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};