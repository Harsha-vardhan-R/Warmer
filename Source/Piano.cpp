/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano() {
    resized();
}

Piano::~Piano() {

}

void Piano::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xffeee3e7));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void Piano::resized() {
    setBounds(150, 20 ,getParentWidth()-150, getParentHeight() - 20);
}