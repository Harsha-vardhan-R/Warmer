/*
  ==============================================================================

    MasterComponent.cpp
    Created: 12 Feb 2024 11:27:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MasterComponent.h"
#include "ModeSwitch.h"

// upperComponent will set the offset in the y-direction at which the
MasterComponent::MasterComponent(int upperComponent) {
    setBounds(0, upperComponent, getParentWidth(), getParentHeight()*0.8f);

    modeSwitch = std::make_unique<ModeSwitch>();
    addAndMakeVisible(modeSwitch.get());
    modeSwitch.get()->setBounds(getLocalBounds());

    resized();
}

MasterComponent::~MasterComponent() {
    modeSwitch.reset(nullptr);
    deleteAllChildren();
}

void MasterComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xff262b2b));
}

void MasterComponent::resized() {
    setSize(getParentWidth(), getParentHeight()*0.8f);
}