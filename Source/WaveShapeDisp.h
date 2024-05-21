/*
  ==============================================================================

    WaveShapeDisp.h
    Created: 5 Mar 2024 11:11:36am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Profiler.h"

//==============================================================================
/*
*/
class WaveShapeDisp : public juce::AudioVisualiserComponent {
public:

    static WaveShapeDisp* this_instance;

    WaveShapeDisp(int);
    ~WaveShapeDisp() override;

    void resized() override;

    void stop();
    void start();

    void trigger();

    void changeBufferSize(int bufferSize);

    void pushFreshBuffer(const juce::AudioBuffer<float>& buffer);

private:
    juce::Colour borderColour;

    juce::CriticalSection lock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShapeDisp)
};
