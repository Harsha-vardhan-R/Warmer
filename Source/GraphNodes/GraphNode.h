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

    // Name of the node(shown at the top)
    juce::String name;

    /*
       PermDependency.
       This is the dependency that is given at the time of building the tree,
       will not be updated with the calls.
    */
    int permDependency;
    /*
     * This is the dependency that will be updated based on the number of processed nodes.
     */
    int dependency;

    /*
        Does this node need an Audio buffer.
        This will be set to true in the constructor of the respective node.
        set with caution as this will be used when creating the priority queue.
    */
    bool needsAudioBuffer;

    /*
        the inplace buffer this has to modify when processing.
        nullptr if it does not need a buffer.
    */
    juce::AudioBuffer<float>* inplaceAudioBuffer;

    int zoomLevel;
    juce::OwnedArray<Socket> InputSockets;
    juce::OwnedArray<Socket> OutputSockets;

    // Height of the node Component.
    // This is not the rendering height, it will depend on the zoom level.
    // It is also the dimensions of the node when the zoom level is 0.
    int UIWidth = 20;
    int UIHeight = 40;

    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y, int x, int y) {

        UIWidth = x;
        UIHeight = y;

        this->name = name;
        setBounds(pos_x, pos_y, UIWidth, UIHeight);
    }

    void setZoomLevel(int level) {
        zoomLevel = level;
    }

    static void deleteNodeCallback() {

    }

    // for moving and delete menu.
    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isRightButtonDown()) {
            // Show context menu
            juce::PopupMenu menu;
            menu.addItem(1, "Delete Node");
            //menu.showMenuAsync(juce::PopupMenu::Options(),
                               //juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
        } else {
            lastMouseDownPosition = event.getPosition();
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override {
        juce::Point<int> delta = event.getPosition() - lastMouseDownPosition;
        auto newBounds = getBounds().translated(delta.getX(), delta.getY());
        setBounds(newBounds);
    }


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g) {
        auto bounds = g.getClipBounds();

        g.setColour(GraphNodeHeadingColourID);
        juce::Rectangle<int> topTextArea(0, 0, getWidth(), 20);
        g.fillRect(topTextArea);

        g.setColour(GraphNodeHeadingTextID);
        g.drawText(name, topTextArea.toFloat(), juce::Justification::centred);

        juce::Rectangle<int> area;

    }

    virtual ~GraphNode() override {}

    // Abstraction for the node, this is called when we are going through the priority queue, and it
    // should do the necessary depending on the present input types.
    // going to rewrite the vector of values or inplace modify the audio buffer.
    virtual void process() {};

    void paint(juce::Graphics& g) override {
        g.fillAll(GraphNodeBackgroundColourID);
        g.setColour(juce::Colours::white);
        paintBasic(g);
    }

    void resized() override {
        setBounds(getLocalBounds().getX(), getLocalBounds().getY(), UIWidth, UIHeight);
    }

private:
    juce::Point<int> lastMouseDownPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};