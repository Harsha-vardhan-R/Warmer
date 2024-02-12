/*
  ==============================================================================

    MasterComponent.cpp
    Created: 12 Feb 2024 11:27:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MasterComponent.h"

MasterComponent::MasterComponent(int upperComponent) {
    setBounds(0, upperComponent, getParentWidth(), 45);
    resized();
}

MasterComponent::~MasterComponent() {}

void MasterComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour());
}

void MasterComponent::resized() {
    setSize(getParentWidth(), 45);
}