/*
  ==============================================================================

    MasterComponent.cpp
    Created: 12 Feb 2024 11:27:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MasterComponent.h"
#include "ModeSwitch.h"
#include "PresetSwitch.h"

// upperComponent will set the offset in the y-direction at which the
MasterComponent::MasterComponent(int upperComponent, int otherComponentCumulativeHeight) {
    this->upperComponent = upperComponent;
    this->otherComponentCumulativeHeight = otherComponentCumulativeHeight;

    setBounds(0, this->upperComponent, getParentWidth(), getParentHeight()-otherComponentCumulativeHeight);

    modeSwitch = std::make_unique<ModeSwitch>();
    modeSwitch.get()->setOutline(1);
    modeSwitch.get()->setIndent(1);
    modeSwitch.get()->setColour(juce::TabbedComponent::ColourIds::backgroundColourId, juce::Colour(0xffeee3e7));
    modeSwitch.get()->setColour(juce::TabbedComponent::ColourIds::outlineColourId, juce::Colours::grey);
    addAndMakeVisible(modeSwitch.get());

    presetSwitch = std::make_unique<PresetSwitch>();
    addAndMakeVisible(presetSwitch.get());

    resized();

}

MasterComponent::~MasterComponent() {}

void MasterComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xff262b2b));
}

void MasterComponent::resized() {
    if (getParentComponent() != nullptr) {
        int wid = getParentComponent()->getLocalBounds().getWidth();
        int hig = getParentComponent()->getLocalBounds().getHeight();

        setSize(wid, hig-otherComponentCumulativeHeight);
        setBounds(0, this->upperComponent, wid, hig-otherComponentCumulativeHeight);
    }
    if (modeSwitch.get() != nullptr) modeSwitch.get()->resized();
    if (presetSwitch.get() != nullptr) presetSwitch.get()->resized();
}