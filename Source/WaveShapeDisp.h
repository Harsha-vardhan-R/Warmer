/*
  ==============================================================================

    WaveShapeDisp.h
    Created: 5 Mar 2024 11:11:36am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveShapeDisp  : public juce::Component {
public:
    WaveShapeDisp();
    ~WaveShapeDisp() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShapeDisp)
};
