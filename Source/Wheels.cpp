/*
  ==============================================================================

    Wheels.cpp
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Wheels.h"
#include "ColourPalette.h"
#include <cmath>

void* Wheels::instancePtr = nullptr;


Wheels::Wheels() {
    this->modWheelLevel = 0.0;
    this->pitchWheelLevel = 0.5;
    this->transpose = 0;

    offset = 0;

    wheelAreas.add(new juce::Rectangle<int>(105+offset, 10, 25, getParentHeight() - 40));
    wheelAreas.add(new juce::Rectangle<int>(155+offset, 10, 25, getParentHeight() - 40));
    // for up transpose and low transpose
    wheelAreas.add(new juce::Rectangle<int>(55+offset, 10, 45, 60));
    wheelAreas.add(new juce::Rectangle<int>(55+offset, 50, 45, 60));

    resized();

    Wheels::instancePtr = this;
}

Wheels::~Wheels() {

}

// helping functions to draw wheel with the present level and the bounds at which it is located.
void drawWheelWithLevel(juce::Rectangle<float> bounds, // wheel bounds
                        juce::Graphics &g,
                        float level) { // level varies from 0.0 to 1.0

    g.setColour(WheelBackgroundID);
    g.fillRoundedRectangle(bounds.toFloat(), 3.0f);
//    g.fillRect(bounds);

    // draw a thin rectangle, then draw the notch.
    float notchCentre= ((float)bounds.getHeight()*0.96)*(1.0-level);
    g.setColour(WheelSliderBackgroundID);
    g.fillRoundedRectangle(bounds.getX() + 1, bounds.getY()+(int)notchCentre, bounds.getWidth()-2, 5.0, 2.0f);
    g.setColour(WheelSliderLineBackgroundID);
    g.fillRect(bounds.getX() + 2, bounds.getY()+(int)notchCentre+2, bounds.getWidth()-4, 1.0);
    g.setColour(WheelSliderLineBackgroundID);g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds.toFloat(), 3.0f, 0.2f);
}

void Wheels::paint(juce::Graphics &g) {
    g.fillAll(WheelComponentBackgroundID);

    juce::Rectangle<int> pitchWheel(105+offset, 10, 25, getParentHeight() - 20);
    juce::Rectangle<int> modWheel(155+offset, 10, 25, getParentHeight() - 20);

    drawWheelWithLevel(modWheel.toFloat(), g, this->modWheelLevel);
    drawWheelWithLevel(pitchWheel.toFloat(), g, this->pitchWheelLevel);

    g.setColour(WheelArrowColourID);

    juce::Path arrow;
    arrow.addTriangle(185+offset, 10,
                      190+offset, 10,
                      185+offset, pitchWheel.getY()+pitchWheel.getHeight());
    g.fillPath(arrow);

    juce::Path arrow2;
    arrow2.addTriangle(140+offset, 10,
                      145+offset, 10,
                      140+offset, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    arrow2.addTriangle(140+offset, getParentHeight() - 10,
                       135+offset, getParentHeight() - 10,
                       140+offset, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    g.fillPath(arrow2);


    juce::Rectangle<int> transposeArea(57+offset, 10, 40, pitchWheel.getHeight());
    juce::String transpose_(this->transpose);
    g.setFont(24.0f);
    g.drawText(transpose_, transposeArea, juce::Justification::centred);

    juce::Path upArrow;
    juce::Path lowArrow;
    lowArrow.addTriangle(72+offset, 85,
                         82+offset, 85,
                         77+offset, 95);
    upArrow.addTriangle(77+offset, 30,
                        72+offset, 40,
                        82+offset, 40);


    g.fillPath(upArrow);
    g.fillPath(lowArrow);

    g.setColour(juce::Colours::grey);
    g.fillRect(getWidth()-2, 0, 2, getHeight());

}

void Wheels::resized() {
    setBounds(0, 0 ,200+offset, getParentHeight());

    wheelAreas[0]->setBounds(105+offset, 10, 25, getParentHeight() - 20);
    wheelAreas[1]->setBounds(155+offset, 10, 25, getParentHeight() - 20);

    wheelAreas[2]->setBounds(55+offset, 10, 45, 60);
    wheelAreas[3]->setBounds(55+offset, 50, 45, 60);
}


void Wheels::setModWheel(float value) {
    modWheelLevel = value;
    repaint();
}


void Wheels::setPitchWheel(float value) {
    pitchWheelLevel = value;
    repaint();
}