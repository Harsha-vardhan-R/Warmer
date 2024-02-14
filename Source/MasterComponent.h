/*
  ==============================================================================

    MasterComponent.h
    Created: 12 Feb 2024 11:27:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ModeSwitch.h"
#include "PresetSwitch.h"


class MasterComponent : public juce::Component {
public:

    MasterComponent(int upperComponent, int otherComponentCumulativeHeight);
    ~MasterComponent() override;
    //++++++++++++++++++++++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int upperComponent;
    int otherComponentCumulativeHeight;

    std::unique_ptr<PresetSwitch> presetSwitch;
    std::unique_ptr<ModeSwitch> modeSwitch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterComponent)
};