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
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void WaveShapeDisp::resized()
{

}
