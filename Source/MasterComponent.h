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

//
class MasterComponent : public juce::Component {
public:

    MasterComponent(int upperComponent);
    ~MasterComponent() override;
    //++++++++++++++++++++++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<ModeSwitch> modeSwitch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterComponent)
};