/*
  ==============================================================================

    Profiler.h
    Created: 20 May 2024 1:29:07pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <juce_core/juce_core.h>


class Profiler {

public:

    Profiler(const juce::String& name)
            : sectionName(name), startTime(juce::Time::getMillisecondCounterHiRes()) {}

    ~Profiler() {
        auto endTime = juce::Time::getMillisecondCounterHiRes();
        auto elapsedTime = endTime - startTime;
        DBG(sectionName + " took " + juce::String(elapsedTime) + " ms");
    }

private:

    juce::String sectionName;

    double startTime;

};
