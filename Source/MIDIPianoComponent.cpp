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

    if (wheelComponent != nullptr) {
        wheelComponent->setOffset(((getWidth()-1400)/2)-50);
        wheelComponent->resized();
    }
    if (pianoComponent != nullptr) {
        pianoComponent->setOffset(((getWidth()-1400)/2)-50);
        pianoComponent->resized();
    }
    if (sideBar != nullptr) {
        sideBar->setOffset(((getWidth()-1400)/2)+50);
        sideBar->resized();
    }

}