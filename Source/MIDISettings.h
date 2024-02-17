/*
  ==============================================================================

    MIDISettings.h
    Created: 15 Feb 2024 7:22:37am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MIDISettings : public juce::Component,
                     public juce::Button::Listener,
                     public juce::ComboBox::Listener {
public:

    MIDISettings();
    ~MIDISettings() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* b) override;

    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    std::unique_ptr<juce::TextButton> myCheckbox;
    std::unique_ptr<juce::ComboBox> comboBox;
    std::unique_ptr<juce::LookAndFeel_V4> styles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISettings)
};