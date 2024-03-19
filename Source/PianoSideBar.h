/*
  ==============================================================================

    PianoSideBar.h
    Created: 12 Mar 2024 2:27:27pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColourPalette.h"


class PianoSideBar  : public juce::Component
{
public:
    PianoSideBar() {}

    ~PianoSideBar() override {}

    void paint (juce::Graphics& g) override {
        g.fillAll(WheelComponentBackgroundID);
        g.setColour(juce::Colours::grey);
        g.fillRect(0, 0, 2, getHeight());
    }

    void resized() override {
        setBounds(getParentWidth()-offset, 0, offset, getParentHeight());
    }

    void setOffset(int n) {
        offset = n;
    }

private:
    int offset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoSideBar)
};
