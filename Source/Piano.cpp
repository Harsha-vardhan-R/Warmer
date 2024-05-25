/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"

int Wheels::TRANSPOSE = 0;

Piano* Piano::Instance_Pointer = nullptr;

Piano::Piano(juce::MidiKeyboardState& state, juce::KeyboardComponentBase::Orientation orientation)  : juce::MidiKeyboardComponent(state, orientation) {

    Piano::Instance_Pointer = this;

    setVelocity(0.75, true);

    //grabKeyboardFocus();

    setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colours::white);
    setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour::fromRGB(100, 100, 100));
    setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colour(0x22000000));
    setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colours::transparentWhite);
    setColour(juce::MidiKeyboardComponent::textLabelColourId, juce::Colours::grey);
    setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colours::darkgrey);

    resized();
}

Piano::~Piano(){}
//
//void Piano::paint(juce::Graphics &g) {
//    g.drawImage(PianoImage, 0 , 0, 1192, getHeight(),
//                0, 0, 1192, 125, false);
//}

void Piano::resized() {
    setBounds(200+offset, 0 ,1192, getParentHeight());
}
