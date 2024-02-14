/*
  ==============================================================================

    PresetSwitch.h
    Created: 12 Feb 2024 11:48:41pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/*
    The class that handles selecting various instruments and various presets if the instruments have them available,
    This bar also contains the Final Gain adjuster and a Spectrometer,
    Both are inspired from Vital and both will look like nearly same as how Vital has them.
 */
class PresetSwitch : public juce::Component {
public:

    PresetSwitch() {
        setSize(getParentWidth(), 35);
    };

    ~PresetSwitch() override {};

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::brown);
    };
    void resized() override {
        setSize(getParentWidth(), 35);
    };

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetSwitch)
};
