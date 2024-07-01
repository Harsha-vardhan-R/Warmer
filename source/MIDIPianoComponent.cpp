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
#include <atomic>

// This is always going to be of the same height
// The offset is to tell how low it should start.
MIDIPianoComponent::MIDIPianoComponent(int offset, int height) :
        pianoComponent(Instrument::getInstance()->keyboardState, juce::KeyboardComponentBase::Orientation::horizontalKeyboard, ((getWidth()-1400)/2)-50) {

    this->offset = offset;
    this->height = height;

    addAndMakeVisible(wheelComponent);

    addAndMakeVisible(pianoComponent);


    addAndMakeVisible(sideBar);

    resized();

}

MIDIPianoComponent::~MIDIPianoComponent() = default;

void MIDIPianoComponent::paint(juce::Graphics &g) {}

void MIDIPianoComponent::resized() {
    this->offset = getParentHeight() - this->height;
    setBounds(0, this->offset, getParentWidth(), this->height);
    setSize(getParentWidth(), this->height);

    wheelComponent.setOffset(((getWidth()-1400)/2)-50);
    wheelComponent.resized();

    pianoComponent.setOffset(((getWidth()-1400)/2)-50);
    pianoComponent.resized();

    sideBar.setOffset(((getWidth()-1400)/2)+50);
    sideBar.resized();

}
