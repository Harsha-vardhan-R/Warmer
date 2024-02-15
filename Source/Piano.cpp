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
    g.fillAll(juce::Colours::darkolivegreen);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 13.0f);
}

void Piano::resized() {
    setBounds(150, 20 ,getParentWidth()-150, getParentHeight() - 20);
    std::cout << getParentHeight() << "\n";
}