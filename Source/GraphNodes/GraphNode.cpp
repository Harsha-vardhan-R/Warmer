/*
  ==============================================================================

    GraphNode.cpp
    Created: 8 Apr 2024 4:27:10pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "GraphNode.h"
#include "../Instrument.h"


std::vector<GraphNode*> GraphNode::getDependencies() {
    std::vector<GraphNode*> output;

    for (auto i : InputSockets) {
        if (i->getPrevNode() != nullptr) output.push_back(static_cast<GraphNode*>(i->getPrevNode()));
    }

    return output;
}


std::set<GraphNode*> GraphNode::getDependents() {
    std::set<GraphNode*> output;

    for (auto i : OutputSockets) {
        for (Socket* j : i->to) output.insert(static_cast<GraphNode*>(j->getParentComponent()));
    }

    return output;
}

std::set<GraphNode*> GraphNode::getAudioBufferDependencies() {
    std::set<GraphNode*> output;

    // we return the set of nodes connected to the first socket of
    // the type AudioBufferFloat in OutputSockets.
    for (Socket* i : OutputSockets) {
        if (i->getOutputType() == SocketDataType::AudioBufferFloat) {
            for (Socket *j: i->to) output.insert(static_cast<GraphNode*>(j->getParentComponent()));
            break;
        }
    }

    return output;
}


GraphNode::GraphNode(juce::String name, int pos_x, int pos_y) {
    setLookAndFeel(&style);

    this->name = name;

    UIWidth = 0;
    UIHeight = 0;

    setBounds(pos_x, pos_y, UIWidth, UIHeight);

    canBeDeleted = true; // for all the nodes by default, will be set to false in the input and output nodes.


    menu.addItem(1, "Delete Node");

    resized();
    repaint();
}

// returns true if all the Input Sockets that are `isMust` are connected.
bool GraphNode::allGood() {
    for (auto i : InputSockets) {
        // if it is a must but not connected we are going to return false.
        if (i->isMust && !(i->isThisConnected())) {
            return false;
        }
    }

    for (auto i : OutputSockets) {
        // if it is a must but not connected we are going to return false.
        if (i->isMust && !(i->isThisConnected())) {
            return false;
        }
    }

    return true;
}

void GraphNode::deleteNodeCallback(int result, GraphNode* graphNodeInstance) {
    if (result == 1) {
        graphNodeInstance->deleteAllConnectionsToAndFromNode();
        Instrument::getInstance()->nodeDeleted(graphNodeInstance);
    }
}

// for moving and delete menu.
void GraphNode::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        if (!canBeDeleted) return;
        // Show context menu
        menu.showMenuAsync(juce::PopupMenu::Options(),
                           juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
    } else {
        lastMouseDownPosition = event.getPosition();
        resized();
    }
}

void GraphNode::mouseDrag(const juce::MouseEvent& event) {
    juce::Point<int> delta = event.getPosition() - lastMouseDownPosition;
    auto newBounds = getBounds().translated(delta.getX(), delta.getY());
    setBounds(newBounds);

    // mouse drag triggers the repainting of connections to the node.
    for (Socket* i : InputSockets) {
        i->repaintConnection();
    }

    for (Socket* i : OutputSockets) {
        i->repaintConnection();
    }


}

void GraphNode::deleteAllConnectionsToAndFromNode() {

    // mouse drag triggers the repainting of connections to the node.
    for (Socket* i : InputSockets) {
        i->deleteConnections();
    }

    for (Socket* i : OutputSockets) {
        i->deleteConnections();
    }
}

// Draws the common parts for all the nodes,
// called before drawing the respective node related content.
void GraphNode::paintBasic(juce::Graphics& g) {
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
void GraphNode::makeAllSocketsVisible() {
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

void GraphNode::paint(juce::Graphics& g) {
    paintBasic(g);
}

void GraphNode::resized() {
    UIWidth = 120;

    int index = 0;
    int end = 30;
    // setting the bounds for each socket.
    // output sockets are displayed at the top, followed by input sockets.
    for (auto i : OutputSockets) {
        i->setBounds(0, 30+(index*20), UIWidth, 20);
        end = (index*20)+50;
        index++;
    }

    end += 5;

    index = 0;
    for (auto i : InputSockets) {
        bool connected = !(i->isThisConnected() || !i->hasParameterController());
        int height = connected*(50) + (1-connected)*20;
        i->setBounds(0, end, UIWidth, height);
        end += height;
        index++;
    }


    UIHeight = end + 10;
    setBounds(getX(), getY(), UIWidth, UIHeight);

}

GraphNode::~GraphNode() {
    setLookAndFeel(nullptr);
}

// loading and saving of presets.
void GraphNode::getStateInformation(juce::MemoryBlock& destData) {

}

void GraphNode::setStateInformation(const void* data, int sizeInBytes) {

}

///|=========================================|
const juce::String GraphNode::getName() const {
    return name; // Replace with your node name
}

double GraphNode::getTailLengthSeconds() const {
    return 0.0; // No tail
}

bool GraphNode::acceptsMidi() const {
    return false; // Change as needed
}

bool GraphNode::producesMidi() const {
    return false; // Change as needed
}

juce::AudioProcessorEditor* GraphNode::createEditor() {
    // Create and return your custom editor
    return nullptr; // Return nullptr if no editor
}

bool GraphNode::hasEditor() const {
    return false; // Change as needed
}

int GraphNode::getNumPrograms() {
    return 0; // No programs
}

int GraphNode::getCurrentProgram() {
    return 0; // No programs
}

void GraphNode::setCurrentProgram(int index) {
    // Handle setting current program
}

const juce::String GraphNode::getProgramName(int index) {
    return {}; // No programs
}

void GraphNode::changeProgramName(int index, const juce::String& newName) {
    // Handle changing program name
}

