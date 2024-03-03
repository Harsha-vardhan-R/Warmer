/*
  ==============================================================================

    MIDIPianoComponent.cpp
    Created: 14 Feb 2024 5:29:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MIDIPianoComponent.h"
#include "MIDISettings.h"
#include "Piano.h"
#include "Instrument.h"

// This is always going to be of the same height
// The offset is to tell how low it should start.
MIDIPianoComponent::MIDIPianoComponent(int offset, int height) {
    this->offset = offset;
    this->height = height;

    midiSettings = std::make_unique<MIDISettings>();
    addAndMakeVisible(midiSettings.get());

    wheelComponent = std::make_unique<Wheels>();
    addAndMakeVisible(wheelComponent.get());

    pianoComponent = std::make_unique<Piano>();
    addAndMakeVisible(pianoComponent.get());

    resized();

}

MIDIPianoComponent::~MIDIPianoComponent() {}

void MIDIPianoComponent::paint(juce::Graphics &g) {
    juce::Rectangle<int> area(getLocalBounds());
    g.setColour(juce::Colours::darkgrey);
    g.fillRoundedRectangle(area.toFloat(), 10.0f);
}

void MIDIPianoComponent::resized() {
    this->offset = getParentHeight() - this->height;
    setBounds(0, this->offset, getParentWidth(), this->height);
    setSize(getParentWidth(), this->height);

    if (midiSettings.get() != nullptr) midiSettings.get()->resized();
    if (wheelComponent.get() != nullptr) wheelComponent.get()->resized();
    if (pianoComponent.get() != nullptr) {
        pianoComponent.get()->resized();
        Instrument::VoidPointerToPianoComponent = (void*)pianoComponent.get();
    }

}