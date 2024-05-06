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


class EmptyLookAndFeel : public juce::LookAndFeel_V4 {
public:

    EmptyLookAndFeel() {
        setColour(juce::Slider::ColourIds::backgroundColourId, GraphSliderBackgroundID);
        setColour(juce::Slider::ColourIds::thumbColourId, GraphSliderThumbID);
        setColour(juce::Slider::ColourIds::trackColourId, GraphSliderOutlineID);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, GraphSliderTextID);

        setColour(juce::ComboBox::ColourIds::backgroundColourId, MenuBackgroundID);
        setColour(juce::ComboBox::ColourIds::textColourId, textColourID);
        setColour(juce::ComboBox::ColourIds::arrowColourId, ComboBoxArrowColourID);
    }

    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                     int buttonW, int buttonH, juce::ComboBox& box) override {
        g.setColour(GraphSliderOutlineID);
        g.drawRoundedRectangle(box.getLocalBounds().toFloat() ,3.0f, 2.0);
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(box.getLocalBounds().reduced(2).toFloat(), 3.0f);

        juce::Path arrow;
        float arrowDimen = 5;
        float arrowX = width - 15; // X position of the arrow
        float arrowY = (height - arrowDimen) / 2; // Y position of the arrow

        if (box.isPopupActive()) {
            arrow.addTriangle(arrowX + (arrowDimen * 0.5f) + 1, arrowY,
                              arrowX, arrowY + arrowDimen,
                              arrowX + arrowDimen + 2, arrowY + arrowDimen);
        } else {
            arrow.addTriangle(arrowX, arrowY,
                              arrowX + arrowDimen + 2, arrowY,
                              arrowX + (arrowDimen * 0.5f) + 1, arrowY + arrowDimen);
        }

        g.setColour(ComboBoxArrowColourID);
        g.fillPath(arrow);
    }

    void drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator,
                                           bool isActive,
                                           bool isHighlighted,
                                           bool isTicked,
                                           bool hasSubMenu,
                                           const juce::String& text,
                                           const juce::String& shortcutKeyText,
                                           const juce::Drawable* icon,
                                           const juce::Colour* textColourToUse) override {

        juce::Rectangle<int> highLight(area.getX() + 3, area.getY(), area.getWidth() - 6 , area.getHeight());
        juce::Rectangle<int> aligner(area.getX() + 15, area.getY(), area.getWidth() , area.getHeight());

        auto width = area.getWidth();


        if (isSeparator) {
            g.setColour(SeparatorColourID);
            juce::Rectangle<int> sepLine(area.getX()+3, area.getCentreY(), area.getWidth()-6, 1);
            g.fillRect(sepLine);
            return;
        } else if (isHighlighted) {
            g.setColour(MenuMouseOverColourID);
            g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
            g.setColour(SelectedTabTextColourID);
        } else {
            g.setColour(IdleTabTextColourID);
        }

        // Draw a side arrow if there is a submenu.
        if (hasSubMenu) {
            juce::Path arrow;
            arrow.addTriangle( width - 12,  6,
                               width - 6,  10,
                               width - 12,  14);
            g.setColour(ComboBoxArrowColourID);
            g.fillPath(arrow);
        }

        g.drawText(text, aligner, juce::Justification::centredLeft);

    }

    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override {
        g.setColour(MenuBackgroundID);
        g.fillAll();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyLookAndFeel)
};


// Class that all the nodes inherit.
class GraphNode : public juce::Component,
                  public juce::AudioProcessor {
public :

    // Name of the node(shown at the top)
    juce::String name;

    /*
     * Returns a vector of dependent nodes.
     */
    std::vector<GraphNode*> getDependencies();

    // returns a vector of values that contains all the nodes this is output to.
    std::set<GraphNode*> getDependents();

    // returns the Nodes that are dependent on the AudioBuffer that is written on by this node,
    // if none exist we are going to return an empty vector.
    std::set<GraphNode*> getAudioBufferDependencies();


    int zoomLevel;
    juce::OwnedArray<Socket> InputSockets;
    // YOU CAN HAVE AT-MOST ONE SOCKET OF THE TYPE SocketDataType::AudioBufferFloat
    juce::OwnedArray<Socket> OutputSockets;

    // The dimensions will be decided by the number of sockets.
    int UIWidth, UIHeight;

    // appears if you want to delete the node.
    juce::PopupMenu menu;


    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y);

    // returns true if all the Input Sockets that are `isMust` are connected.
    virtual bool allGood();

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

    // the function callback pointer that is

    double sampleRate;
    int estimatedSamplesPerBlock;

    // Pure virtual functions from the juce::AudioProcessor.
    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) {
        this->sampleRate = sampleRate;
        this->estimatedSamplesPerBlock = estimatedSamplesPerBlock;
    }

    // called while destructing the node,
    // be very careful with shared data from the nodes.
    virtual void releaseResources() = 0;

    // this function is never used , we call the function called `processGraphNode`, to process the node.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {}

    // called after the prepareToPlay is called,
    // use it if you want to configure more things after
    // setting the bufferSize and rate.
    //======
    // this should be used to set the call-back function pointer and the respective data that is,
    // needed for the respective callback function to work.
    //======
    // you basically manage the data while creating and removing a connection.
    // This function will be called every time a connection is added or removed from
    // any og the sockets connected to this
    //=====
    // the audio buffer that needs to get written will get the
    virtual void reset() = 0;


    juce::AudioBuffer<float>* bufferToWritePointer;
    // set false if this node does not send out an audio buffer as an output.
    bool needAudioBuffer = true;


    void setToWriteAudioBuffer(juce::AudioBuffer<float>* b) { bufferToWritePointer = b; }

    // the master function that is called from the AudioThread fo each audio buffer,
    // Make sure anything you access from this function is either thread synchronised,
    // or you are 100% sure that it is not going to be accessed from any other thread.
    //
    // The connection has all the stuff you just need, just ask it.
    virtual void processGraphNode() = 0;

    // loading and saving of presets.
    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    // lock and unlock while reading the values.
    void lock();

    void unlock();


    ///|=========================================|
    // Virtual functions that have their definitions empty.
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

    EmptyLookAndFeel style;

    juce::Point<int> lastMouseDownPosition;

    // locking from other Node's process.
    std::mutex mutex;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
