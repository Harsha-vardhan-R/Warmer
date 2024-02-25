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

// Class that all the nodes inherit.

// Assuming each node can has maximum of 5 input sockets and 3 output sockets.
// the names of the socket will be
class GraphNode : public juce::Component {
public :

    // Height of the node Component.
    // This is not the rendering height, it will depend on the zoom level.
    int UIWidth = 20;
    int UIHeight = 40;

    //Location in the 2d graph.
    int UI_X;
    int UI_Y;


    /// Input and Output sockets of the types : "InputTypesForSocket" and "OutputTypesForSocket"
    SocketDataType INSocket[5] = {
            SocketDataType::NULLType,
            SocketDataType::NULLType,
            SocketDataType::NULLType,
            SocketDataType::NULLType,
            SocketDataType::NULLType
    };
    bool isInputSocketMust[5] = {false, false, false, false, false};// we will not solve the path if one of the node in the device
    // in the device solve path if one of the must sockets is not connected.
    juce::String InputSocketNames[5] = {
            juce::String(""),
            juce::String(""),
            juce::String(""),
            juce::String(""),
            juce::String(""),
    };

    SocketDataType OUTSocket[3] = {
            SocketDataType::NULLType,
            SocketDataType::NULLType,
            SocketDataType::NULLType
    };
    juce::String OutputSocketNames[5] = {
            juce::String(""),
            juce::String(""),
            juce::String(""),
    };

    // Name of the node(shown at the top)
    juce::String name;

    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x = 0, int pos_y = 0, int x = 20, int y = 40) {
        UI_X = pos_x;
        UI_Y = pos_y;

        UIWidth = x;
        UIHeight = y;

        this->name = name;

        resized();
    }


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g ,int zoomLevel) {
        auto bounds = g.getClipBounds();
        g.setColour(GraphNodeBackgroundColourID);
        g.fillRoundedRectangle(bounds.toFloat().reduced(2), 3.0f);
        g.setColour(GraphNodeHeadingTextID);
        g.drawText(name, bounds, juce::Justification::centredTop);

        // Draw the sockets.
        for (int i = 0; i < 5, INSocket[i] != SocketDataType::NULLType; i++) {
            // else draw a circle with the type colour and draw a white border if it is important
            switch (INSocket[i]) {
                case SocketDataType::MIDI:
                    g.setColour(GraphNodeMIDIColourID);
                case SocketDataType::AudioBufferInt:
                    g.setColour(GraphNodeAudioBufferIntColourID);
                case SocketDataType::AudioBufferFloat:
                    g.setColour(GraphNodeAudioBufferFloatColourID);
                case SocketDataType::Integer:
                    g.setColour(GraphNodeIntegerColourID);
                case SocketDataType::Floating:
                    g.setColour(GraphNodeFloatingColourID);
            }
            g.fillRoundedRectangle(bounds.getX(), bounds.getY()+(i*10)+20, 4, 4, 2.0);
            g.setColour(juce::Colour(GraphNodeImpColourID));
            if (isInputSocketMust[i]) {
                g.drawRect(bounds.getX(), bounds.getY()+(i*10)+20, 4, 4);
            }
            // write the name of the socket.
            g.setColour(juce::Colours::white);
            juce::Rectangle<int> NewBounds(bounds.getX()+10, bounds.getX()+(i*15)+15, 40, 10);
            g.drawText(InputSocketNames[i], NewBounds, juce::Justification::centred);
        }

        for (int i = 0; i < 3; i++) {
            switch (OUTSocket[i]) {
                case SocketDataType::MIDI:
                    g.setColour(GraphNodeMIDIColourID);
                case SocketDataType::AudioBufferInt:
                    g.setColour(GraphNodeAudioBufferIntColourID);
                case SocketDataType::AudioBufferFloat:
                    g.setColour(GraphNodeAudioBufferFloatColourID);
                case SocketDataType::Integer:
                    g.setColour(GraphNodeIntegerColourID);
                case SocketDataType::Floating:
                    g.setColour(GraphNodeFloatingColourID);
            }
            g.fillRoundedRectangle(bounds.getX(), bounds.getY()+(i*10)+20, 4, 4, 2.0);
            g.setColour(juce::Colours::white);
            juce::Rectangle<int> NewBounds(bounds.getX()+bounds.getWidth()-50, bounds.getX()+(i*15)+15, 40, 10);
            g.drawText(InputSocketNames[i], NewBounds, juce::Justification::centred);
        }
    }



    virtual ~GraphNode() override {}

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
    }

    void resized() override {
        setBounds(UI_X, UI_Y, UIWidth, UIHeight);
    }
};