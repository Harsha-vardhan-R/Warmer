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
     * Returns a vector of dependent nodes.
     */
    std::vector<GraphNode*> getDependencies() {
        std::vector<GraphNode*> output;

        for (auto i : InputSockets) {
            if (i->getPrevNode() != nullptr) output.push_back(static_cast<GraphNode*>(i->getPrevNode()));
        }

        return output;
    }

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

    // The dimensions will be decided by the number of sockets.
    int UIWidth, UIHeight;

    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y) {
        this->name = name;

        UIWidth = 0;
        UIHeight = 0;

        setBounds(pos_x, pos_y, UIWidth, UIHeight);

        canBeDeleted = true; // for all the nodes by default, will be set to false in the input and output nodes.
        needsAudioBuffer = true; // set to false in required nodes.

        resized();
    }

    // returns true if all the Input Sockets that are `isMust` are connected.
    bool allGood() {
        for (auto i : InputSockets) {
            // if it is a must but not connected we are going to return false.
            if (i->isMust && !(i->isThisConnected())) {
                return false;
            }
        }

        return true;
    }

    static void deleteNodeCallback(int result, GraphNode* graphNodeInstance) {

    }

    // for moving and delete menu.
    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isRightButtonDown()) {
            // Show context menu
            juce::PopupMenu menu;
            menu.addItem(1, "Delete Node");
            menu.showMenuAsync(juce::PopupMenu::Options(),
                               juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
        } else {
            lastMouseDownPosition = event.getPosition();
            resized();
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
        g.setColour(GraphNodeBackgroundColourID);
        g.fillRect(bounds);

        g.setColour(GraphNodeHeadingColourID);
        juce::Rectangle<int> topTextArea(0, 0, getWidth(), 20);
        g.fillRect(topTextArea);

        g.setColour(GraphNodeHeadingTextID);
        g.drawText(name, topTextArea.toFloat(), juce::Justification::centred);

    }

    // sets all the sockets in both input and output socket arrays to
    // addAndMakeVisible
    void makeAllSocketsVisible() {
        resized(); // set the bounds.

        // draw the sockets.
        for (Socket* i : InputSockets) {
            addAndMakeVisible(i);
            i->update();
        }
        for (Socket* i : OutputSockets) {
            addAndMakeVisible(i);
            i->update();
        }
    }

    ~GraphNode() override {}

    // Abstraction for the node to make necessary updates for the present configuration.
    virtual void update(double bitRate, int bufferSize) {}

    // Abstraction for the node, this is called when we are going through the priority queue, and it
    // should do the necessary depending on the present input types.
    // going to rewrite the vector of values or inplace modify the audio buffer.
    virtual void process() {};

    void paint(juce::Graphics& g) override {
        paintBasic(g);
    }

    void resized() override {
        UIWidth = 120;
        UIHeight = 35+(InputSockets.size()*50 + OutputSockets.size()*25); // each socket is given 50px in height.

        int index = 0;
        // setting the bounds for each socket.
        // output sockets are displayed at the top, followed by input sockets.
        for (auto i : OutputSockets) {
            i->setBounds(0, 30+(index*20), UIWidth, 20);
            index++;
        }

        for (auto i : InputSockets) {
            if (OutputSockets.size() == 0) {
                i->setBounds(0, (index*50)+30, UIWidth, 50);
            } else {
                i->setBounds(0, (index*50), UIWidth, 50);
            }
            index++;
        }

        setSize(UIWidth, UIHeight);
    }

    // lock and unlock while reading the values.
    void lock() { mutex.lock(); }

    void unlock() { mutex.unlock(); }

    bool canBeDeleted;

private:
    juce::Point<int> lastMouseDownPosition;

    // locking from other Node's process.
    std::mutex mutex;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
