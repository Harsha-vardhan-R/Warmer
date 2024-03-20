/*
  ==============================================================================

    VolumeLevel.cpp
    Created: 29 Feb 2024 7:02:30pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "VolumeLevel.h"
#include "ColourPalette.h"



VolumeLevel::VolumeLevel() {
    VolumeLevel::instance = this;
    level = 0.6;

    resized();
}

void VolumeLevel::paint(juce::Graphics& g) {
    g.setColour(VolumeLevelBackgroundID);
    juce::Rectangle<float> levelArea(0, 12, getWidth(), getHeight()-24);
    g.fillRect(levelArea);
    // Draw the present level here.
    // slider.
    int center = level*0.99*getWidth();

    // draw the slider with size 4 px width.
    g.setColour(VolumeSliderID);
    g.fillRoundedRectangle(center-3, 8, 7, getHeight()-16, 2.0f);
    g.setColour(VolumeSliderSlitID);
    g.drawRect(center, 10, 1, getHeight()-20);

}

void VolumeLevel::resized() {
    //setBounds(this->offset, 0, getParentWidth()*0.1, getParentHeight());
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