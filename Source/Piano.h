/*
  ==============================================================================

    Piano.h
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/*
 * Modulation and pitch wheels sit here.
*/

class Piano : public juce::Component {
public:

    Piano();
    ~Piano() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const int blackKeysPosition[37] = { 1,  2,  4,  5,  6,
                                        8,  9, 11, 12, 13,
                                       15, 16, 18, 19, 20,
                                       22, 23, 25, 26, 27,
                                       29, 30, 32, 33, 34,
                                       36, 37, 39, 40, 41,
                                       43, 44, 46, 47, 48,
                                       50, 51};

    const int keyColour[87] = {
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Piano)
};