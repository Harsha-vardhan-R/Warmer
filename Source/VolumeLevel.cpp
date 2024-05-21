/*
  ==============================================================================

    VolumeLevel.cpp
    Created: 29 Feb 2024 7:02:30pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "VolumeLevel.h"
#include "ColourPalette.h"
#include "Instrument.h"



VolumeLevel::VolumeLevel() {
    VolumeLevel::instance = this;

    volumeSlider.setRange(0.0, 1.1224, 0.001);
    volumeSlider.setSliderStyle(juce::Slider::LinearBar);
    // Hide the text box
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 0, 0);
    volumeSlider.setLookAndFeel(&style);
    addAndMakeVisible(&volumeSlider);

    volumeSlider.textFromValueFunction = [](double value) {
        float dbValue = juce::Decibels::gainToDecibels(value) * 12.0f; // Adjust for decibel range
        return "Gain : " + juce::String(dbValue, 2) + " dB";
    };

    // Add listener to handle value changes
    volumeSlider.addListener(this);
    volumeSlider.setValue(0.5f);
}

void VolumeLevel::sliderValueChanged(juce::Slider *slider) {
    if (slider == &volumeSlider) {
        Instrument::getInstance()->setAudioDeviceVolume(slider->getValue());
    }
}


void* VolumeLevel::getInstance() {
    if (instance != nullptr) {
        return (void*)VolumeLevel::instance;
    }
    int h;
    int *b = nullptr;
    std::cout << "Isinstance() called before creating, in the VolumeLevel Class" << std::endl;
    h = *b;
    return nullptr;
}

void VolumeLevel::paint(juce::Graphics &g) {}

void VolumeLevel::resized() {
    volumeSlider.setBounds(getLocalBounds());
    volumeSlider.getTextFromValue(volumeSlider.getValue());
}