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
#include "Socket.h"


// Class that all the nodes inherit.
class GraphNode : public juce::Component,
                  public juce::AudioProcessor {
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
     * This is the dependency that will be updated based on the number of processed nodes.(not used for now)
     */
    int dependency;

    /*
     * Returns a vector of dependent nodes.
     */
    std::vector<GraphNode*> getDependencies();

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

    // appears if you want to delete the node.
    juce::PopupMenu menu;


    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y);

    // returns true if all the Input Sockets that are `isMust` are connected.
    bool allGood();

    static void deleteNodeCallback(int result, GraphNode* graphNodeInstance);

    // for moving and delete menu.
    void mouseDown(const juce::MouseEvent& event) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    // deletes all the connections that come to this node,
    // and are from this node
    void deleteAllConnectionsToAndFromNode();


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g);

    // sets all the sockets in both input and output socket arrays to
    // addAndMakeVisible
    void makeAllSocketsVisible();


    void paint(juce::Graphics& g) override;

    void resized() override;


    ~GraphNode() override;


//    Copy and Implement for your own node.
//
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {}
//    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override {}
//    void releaseResources() override {}
//    void reset() override {}


    // Pure virtual functions from the juce::AudioProcessor.
    virtual void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) = 0;
    virtual void releaseResources() = 0;
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) = 0;
    virtual void reset() = 0;

    // loading and saving of presets.
    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    // lock and unlock while reading the values.
    void lock();

    void unlock();


    ///|=========================================|
    const juce::String getName() const override;

    double getTailLengthSeconds() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String& newName) override;

    // +++++++++++++++++++++++

    bool canBeDeleted;

private:
    juce::Point<int> lastMouseDownPosition;

    // locking from other Node's process.
    std::mutex mutex;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
