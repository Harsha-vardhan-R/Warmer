/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano() {
    resized();
}

Piano::~Piano() {}

void Piano::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::grey);

    int pianoWhiteKeyWidth = 33;
    int pianoBlackKeyWidth = 20;

    char c = '0';

    int offset = 884-(getWidth()/2);
    // number of keys to be displayed in viewport.
    int numOfKeys = (getWidth()+offset)/pianoWhiteKeyWidth;

    for (int i = 0; i < numOfKeys; i++) {

        g.setColour(juce::Colour(0xffeee3e7));
        g.fillRoundedRectangle((pianoWhiteKeyWidth+1)*i-offset, 0, pianoWhiteKeyWidth, getHeight()-4, 4.0f);

        if (i%7 == 0) {
            juce::String noteC("Cz");
            g.setColour(juce::Colours::grey);
            g.drawText(noteC.replaceCharacter('z', c), (pianoWhiteKeyWidth+1)*i-offset, 100, 32, 20, juce::Justification::centred);
            c++;
        }

    }

    g.setColour(juce::Colours::black);

    for (auto i : blackKeysPosition) {
        if (((pianoWhiteKeyWidth + 1) * (i-1)) + 23 - offset > getWidth()) break;
        g.fillRoundedRectangle(((pianoWhiteKeyWidth + 1) * (i-1)) + 23 - offset, 0, pianoBlackKeyWidth, getHeight() - 40, 3.0f);
    }

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 2.5f);

}

void Piano::resized() {
    setBounds(150, 20 ,getParentWidth()-150, getParentHeight() - 20);
}