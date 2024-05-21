/*
  ==============================================================================

    VolumeLevel.h
    Created: 29 Feb 2024 7:02:30pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

class VolumeBarLookAndFeel : public juce::LookAndFeel_V3 {
public:

    VolumeBarLookAndFeel() {
        setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xFF2C3333));
        setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::white);
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(0xFFB9F8D3));
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::grey);
    }

};

class VolumeLevel : public juce::Component,
                    public juce::Slider::Listener {
public :

    static inline VolumeLevel* instance = nullptr;
    juce::Slider volumeSlider;


    VolumeLevel();
    ~VolumeLevel() override {
        volumeSlider.setLookAndFeel(nullptr);
    };

    void sliderValueChanged(juce::Slider* slider) override;

    static void* getInstance();

    void paint(juce::Graphics& g) override;
    void resized() override;



private :
    float level; // set by the user defaults at 0.6. can be from 0.0 to 1.0.

    VolumeBarLookAndFeel style;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeLevel)
};