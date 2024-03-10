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

    wheelAreas.add(new juce::Rectangle<int>(55, 10, 25, getParentHeight() - 40));
    wheelAreas.add(new juce::Rectangle<int>(105, 10, 25, getParentHeight() - 40));
    // for up transpose and low transpose
    wheelAreas.add(new juce::Rectangle<int>(5, 10, 45, 60));
    wheelAreas.add(new juce::Rectangle<int>(5, 50, 45, 60));

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

    juce::Rectangle<int> pitchWheel(55, 10, 25, getParentHeight() - 20);
    juce::Rectangle<int> modWheel(105, 10, 25, getParentHeight() - 20);

    drawWheelWithLevel(modWheel.toFloat(), g, this->modWheelLevel);
    drawWheelWithLevel(pitchWheel.toFloat(), g, this->pitchWheelLevel);

    g.setColour(WheelArrowColourID);

    juce::Path arrow;
    arrow.addTriangle(135, 10,
                      140, 10,
                      135, pitchWheel.getY()+pitchWheel.getHeight());
    g.fillPath(arrow);

    juce::Path arrow2;
    arrow2.addTriangle(90, 10,
                      95, 10,
                      90, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    arrow2.addTriangle(90, getParentHeight() - 10,
                       85, getParentHeight() - 10,
                       90, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    g.fillPath(arrow2);


    juce::Rectangle<int> transposeArea(7, 10, 40, pitchWheel.getHeight());
    juce::String transpose_(this->transpose);
    g.setFont(22.0f);
    g.drawText(transpose_, transposeArea, juce::Justification::centred);

    juce::Path upArrow;
    juce::Path lowArrow;
    lowArrow.addTriangle(22, 85,
                         32, 85,
                         27, 95);
    upArrow.addTriangle(27, 30,
                        22, 40,
                        32, 40);


    g.fillPath(upArrow);
    g.fillPath(lowArrow);

    g.setColour(juce::Colours::grey);
    g.fillRect(148, 0, 2, getHeight());

}

void Wheels::resized() {
    setBounds(0, 0 ,150, getParentHeight());
}


void Wheels::setModWheel(float value) {
    modWheelLevel = value;
    repaint();
}


void Wheels::setPitchWheel(float value) {
    pitchWheelLevel = value;
    repaint();
}