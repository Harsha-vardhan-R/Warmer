/*
  ==============================================================================

    Instrument.cpp
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/


#include "Instrument.h"
#include "Modes.h"

Instrument::Instrument(int tabWidth) {
    this->tabWidth = tabWidth;

    presentMode = Mode::Play;
    resized();
}

Instrument::~Instrument() {}

void Instrument::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);

    switch (this->presentMode) {
        case Mode::Play:
            g.setColour(juce::Colours::grey);
        case Mode::Edit:
            g.setColour(juce::Colours::red);
        case Mode::Graph:
            g.setColour(juce::Colours::white);
    }

    g.fillRoundedRectangle(getLocalBounds().toFloat(), 15.0f);
}


void Instrument::setMode(Mode mode) {
    presentMode = mode;
    repaint();
}

void Instrument::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();

    if (parent != nullptr) setBounds(this->tabWidth,
                                     1,
                                     parent->getWidth() - this->tabWidth - 1,
                                     parent->getHeight() - 2);
}