/*
  ==============================================================================

    Wheels.h
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/*
 * Modulation and pitch wheels sit here.
*/

class Wheels : public juce::Component {
public:

    Wheels();
    ~Wheels() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Wheels)
};