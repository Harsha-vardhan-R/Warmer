/*
  ==============================================================================

    Instrument.cpp
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/


#include "Instrument.h"
#include "Modes.h"
#include "./GraphNodes/Collection.h"
#include "Piano.h"

void* Instrument::VoidPointerToPianoComponent = nullptr;


// This maps from the midi input to my representation,
// anything above 99 is for a black key, whose actual value is (value-100)
int MIDI_TO_MINE[113] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

        0, 100, 1, 101, 2, 3, 102, 4, 103, 5, 104, 6,
        7, 105, 8, 106, 9, 10, 107, 11, 108, 12, 109, 13,
        14, 110, 15, 111, 16, 17, 112, 18, 113, 19, 114, 20,
        21, 115, 22, 116, 23, 24, 117, 25, 118, 26, 119, 27,
        28, 120, 29, 121, 30, 31, 122, 32, 123, 33, 124, 34,
        35, 125, 36, 126, 37, 38, 127, 39, 128, 40, 129, 41,
        42, 130, 43, 131, 44, 45, 132, 46, 133, 47, 134, 48,
        48, 135, 49, 136, 50
};


Instrument::Instrument(int tabWidth) {

    this->tabWidth = tabWidth;

    editPage.reset(new Instrument::EditPage());
    graphPage.reset(new Instrument::GraphPage());
    playPage.reset(new Instrument::PlayPage());

    presentMode = Mode::Play;


    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    // Create and add MidiInput objects to the owned array
    for (const auto& device : midiDevicesHere) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        if (midiInput != nullptr) {
            midiInput->start();// Starting to listen on all the possible MIDI inputs.
            midiInputs.add(std::move(midiInput)); // Use std::move to transfer ownership.
        }
    }

    for (auto i : midiInputs) {
        std::cout << i->getName() << "\n";
    }

    resized();
}

Instrument::~Instrument() {

}

void Instrument::paint(juce::Graphics& g) {}


void Instrument::setMode(Mode mode) {
    presentMode = mode;
    repaint();
}

void Instrument::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();

    if (parent != nullptr) setBounds(this->tabWidth,
                                     1,
                                     parent->getWidth() - this->tabWidth - 1,
                                     parent->getHeight() - 2);

    if (editPage.get()->getParentComponent() != nullptr) {editPage.get()->resized();}
    if (graphPage.get()->getParentComponent() != nullptr) {graphPage.get()->resized();}
    if (playPage.get()->getParentComponent() != nullptr) {playPage.get()->resized();}

}



/////// EDIT PAGE ///////////
Instrument::EditPage::EditPage() {
    resized();
}

void Instrument::EditPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30, 1, parent->getWidth() - 29, parent->getHeight() - 2);
}

void Instrument::EditPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::grey);
}



/////// GRAPH PAGE ///////////
Instrument::GraphPage::GraphPage() {
    resized();
}

void Instrument::GraphPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30, 1, parent->getWidth() - 29, parent->getHeight() - 2);
}

void Instrument::GraphPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::white);
}


/////// VIEW PAGE ///////////
Instrument::PlayPage::PlayPage() {
    resized();
}

void Instrument::PlayPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30,
              1,
              parent->getWidth() - 29,
              parent->getHeight() - 2);
}

void Instrument::PlayPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::aqua);
}

void Instrument::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {

    // Lock the message because we are calling from a different thread(most probably).
    juce::MessageManagerLock _messageManagerLock;

    if (Instrument::VoidPointerToPianoComponent == nullptr) {
        return;
    }

    auto PianoComponent = (Piano*)Instrument::VoidPointerToPianoComponent;

    if (message.isNoteOn()) {
        int noteNumber = message.getNoteNumber();
        int velocity = message.getVelocity();
        int midi_ = MIDI_TO_MINE[noteNumber];

        if (midi_ > 99) {
            PianoComponent->overlayPainter.get()->BlackKeyDown(midi_-100);
        } else if (midi_ > -1) {
            PianoComponent->overlayPainter.get()->WhiteKeyDown(midi_);
        }

    } else if (message.isNoteOff()) {
        int noteNumber = message.getNoteNumber();
        int midi_ = MIDI_TO_MINE[noteNumber];

        if (midi_ > 99) {
            PianoComponent->overlayPainter.get()->BlackKeyUp(midi_-100);
        } else if (midi_ > -1) {
            PianoComponent->overlayPainter.get()->WhiteKeyUp(midi_);
        }
    }

    PianoComponent->overlayPainter.get()->repaint();
}




