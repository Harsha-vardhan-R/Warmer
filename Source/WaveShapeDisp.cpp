/*
  ==============================================================================

    WaveShapeDisp.cpp
    Created: 5 Mar 2024 11:11:36am
    Author:  harshavardhan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveShapeDisp.h"

//==============================================================================
WaveShapeDisp::WaveShapeDisp()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WaveShapeDisp::~WaveShapeDisp() {}

void WaveShapeDisp::paint (juce::Graphics& g) {
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0, 1);
}

void WaveShapeDisp::resized()
{

}
