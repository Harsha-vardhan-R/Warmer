/*
  ==============================================================================

    VolumeLevel.cpp
    Created: 29 Feb 2024 7:02:30pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "VolumeLevel.h"



VolumeLevel::VolumeLevel(int offset) {
    this->offset = offset;
    resized();
}

void VolumeLevel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void VolumeLevel::resized() {
    setBounds(this->offset, 0, getParentWidth()*0.1, getParentHeight());
    VolumeLevel::instance = this;
}

void *VolumeLevel::getInstance() {
    if (instance != nullptr) {
        return (void*)VolumeLevel::instance;
    }
    int h;
    int *b = nullptr;
    std::cout << "Isinstance() called before creating, in the VolumeLevel Class" << std::endl;
    h = *b;
    return nullptr;
}