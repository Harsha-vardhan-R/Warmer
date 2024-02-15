/*
  ==============================================================================

    Wheels.cpp
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Wheels.h"


Wheels::Wheels() {
    resized();
}

Wheels::~Wheels() {

}

void Wheels::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::darkolivegreen);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 13.0f);
}

void Wheels::resized() {
    setBounds(0, 20 ,150, getParentHeight() - 20);
    std::cout << getParentHeight() << "\n";
}
