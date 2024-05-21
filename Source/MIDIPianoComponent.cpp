/*
  ==============================================================================

    MIDIPianoComponent.cpp
    Created: 14 Feb 2024 5:29:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MIDIPianoComponent.h"
#include "Piano.h"
#include "Instrument.h"

// This is always going to be of the same height
// The offset is to tell how low it should start.
MIDIPianoComponent::MIDIPianoComponent(int offset, int height) {
    this->offset = offset;
    this->height = height;

    wheelComponent = std::make_unique<Wheels>();
    addAndMakeVisible(wheelComponent.get());

    pianoComponent = std::make_unique<Piano>(keyboardState, juce::KeyboardComponentBase::Orientation::horizontalKeyboard);
    addAndMakeVisible(pianoComponent.get());
    Instrument::VoidPointerToPianoComponent = (void*)pianoComponent.get();

    sideBar.reset(new PianoSideBar());
    addAndMakeVisible(sideBar.get());

    resized();

}

MIDIPianoComponent::~MIDIPianoComponent() {}

void MIDIPianoComponent::paint(juce::Graphics &g) {}

void MIDIPianoComponent::resized() {
    this->offset = getParentHeight() - this->height;
    setBounds(0, this->offset, getParentWidth(), this->height);
    setSize(getParentWidth(), this->height);

    if (wheelComponent.get() != nullptr) {
        wheelComponent.get()->setOffset(((getWidth()-1392)/2)-50);
        wheelComponent.get()->resized();
    }
    if (pianoComponent.get() != nullptr) {
        pianoComponent.get()->setOffset(((getWidth()-1392)/2)-50);
        pianoComponent.get()->resized();
    }
    if (sideBar.get() != nullptr) {
        sideBar.get()->setOffset(((getWidth()-1392)/2)+50);
        sideBar.get()->resized();
    }

}