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

    pianoComponent = std::make_unique<Piano>();
    addAndMakeVisible(pianoComponent.get());
    Instrument::VoidPointerToPianoComponent = (void*)pianoComponent.get();

    resized();

}

MIDIPianoComponent::~MIDIPianoComponent() {}

void MIDIPianoComponent::paint(juce::Graphics &g) {}

void MIDIPianoComponent::resized() {
    this->offset = getParentHeight() - this->height;
    setBounds(0, this->offset, getParentWidth(), this->height);
    setSize(getParentWidth(), this->height);


    if (wheelComponent.get() != nullptr) wheelComponent.get()->resized();
    if (pianoComponent.get() != nullptr) pianoComponent.get()->resized();



}