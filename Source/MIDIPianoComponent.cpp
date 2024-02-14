/*
  ==============================================================================

    MIDIPianoComponent.cpp
    Created: 14 Feb 2024 5:29:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MIDIPianoComponent.h"

// This is always going to be of the same height
// The offset is to tell how low it should start.
MIDIPianoComponent::MIDIPianoComponent(int offset, int height) {
    this->offset = offset;
    this->height = height;
    resized();
}

MIDIPianoComponent::~MIDIPianoComponent() {

}

void MIDIPianoComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::blueviolet);
    juce::Rectangle<int> area(getLocalBounds());
    g.setColour(juce::Colours::darkgrey);
    g.fillRoundedRectangle(area.toFloat(), 10.0f);
}

void MIDIPianoComponent::resized() {
    this->offset = getParentHeight()-75;
    setBounds(0, this->offset, getParentWidth(), this->height);
    setSize(getParentWidth(), this->height);
}