/*
  ==============================================================================

    Instrument.h
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Modes.h"

/*
    Contains all the functions related to Instrument.
    Everything from loading an instrument, saving it, playing it and painting it on the component.
*/
class Instrument : public juce::Component {
public:

    Instrument(int);
    ~Instrument() override;
    //++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;
    //====== INSTRUMENT'S API's ========

    /* Sets the mode and calls the paint function, basically will switch tabs */
    void setMode(Mode mode);

private:
    int size_width, size_height;
    int tabWidth;
    // This will be set before painting, used to render different things from the same component.
    Mode presentMode;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};