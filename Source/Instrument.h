/*
  ==============================================================================

    Instrument.h
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Modes.h"
#include "Piano.h"


/*
    Contains all the functions related to Instrument.
    Everything from loading an instrument, saving it, playing it and painting it on the component.
*/
class Instrument : public juce::Component,
                   public juce::MidiInputCallback {
public:

    // Maintaining a global static variable for the instance pointer,
    // Not a good idea, but refactoring takes years.
    // no memory management required as it will be taken care of.
    static void* VoidPointerToPianoComponent;

    Instrument(int);
    ~Instrument() override;
    //++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;
    //====== INSTRUMENT'S API's ========



    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

    /* Sets the mode and calls the paint function, basically will switch tabs */
    void setMode(Mode mode);

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class EditPage : public juce::Component {
    public:

        EditPage();
        ~EditPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditPage)
    };

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class GraphPage : public juce::Component {
    public:
        GraphPage();
        ~GraphPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphPage)
    };

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class PlayPage : public juce::Component {
    public:
        PlayPage();
        ~PlayPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPage)
    };


    // Because the pages to be displayed are private, get functions for the pages.
    // Please do not use this before creating the pages, or else everything will crash
    juce::Component* getEditPage() {
        return this->editPage.get();
    }

    juce::Component* getGraphPage() {
        return this->graphPage.get();
    }

    juce::Component* getPlayPage() {
        return this->playPage.get();
    }

private:
    std::unique_ptr<juce::Component> editPage;
    std::unique_ptr<juce::Component> graphPage;
    std::unique_ptr<juce::Component> playPage;

    int size_width, size_height;
    int tabWidth;

    Mode presentMode;

    juce::OwnedArray<juce::MidiInput> midiInputs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
