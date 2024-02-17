/*
  ==============================================================================

    Wheels.cpp
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Wheels.h"
#include <cmath>


Wheels::Wheels() {
    this->modWheelLevel = 0.0;
    this->pitchWheelLevel = 0.5;
    this->transpose = 0;

    wheelAreas.add(new juce::Rectangle<int>(55, 10, 25, getParentHeight() - 40));
    wheelAreas.add(new juce::Rectangle<int>(105, 10, 25, getParentHeight() - 40));
    // for up transpose and low transpose
    wheelAreas.add(new juce::Rectangle<int>(5, 10, 45, 40));
    wheelAreas.add(new juce::Rectangle<int>(5, 50, 45, 40));

    resized();
}

Wheels::~Wheels() {

}

// helping functions to draw wheel with the present level and the bounds at which it is located.
void drawWheelWithLevel(juce::Rectangle<float> bounds, // wheel bounds
                        juce::Graphics &g,
                        float level) { // level varies from 0.0 to 1.0
    // draw a thin rectangle, then draw the notch.
    g.setColour(juce::Colour(0xfff6abb6));
    g.fillRoundedRectangle(bounds.toFloat(), 3.0f);
    float notchCentre= ((float)bounds.getHeight()*0.95)*(1.0-level);
    g.setColour(juce::Colour(juce::Colours::grey));
    g.fillRoundedRectangle(bounds.getX() + 1, bounds.getY()+(int)notchCentre, bounds.getWidth()-2, 5.0, 3.0f);
}

void Wheels::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xffeee3e7));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);

    juce::Rectangle<int> pitchWheel(55, 10, 25, getParentHeight() - 40);
    juce::Rectangle<int> modWheel(105, 10, 25, getParentHeight() - 40);

    drawWheelWithLevel(modWheel.toFloat(), g, this->modWheelLevel);
    drawWheelWithLevel(pitchWheel.toFloat(), g, this->pitchWheelLevel);

    juce::Path arrow;
    arrow.addTriangle(135, 10,
                      140, 10,
                      135, pitchWheel.getY()+pitchWheel.getHeight());
    g.fillPath(arrow);

    juce::Path arrow2;
    arrow2.addTriangle(90, 10,
                      95, 10,
                      90, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    arrow2.addTriangle(90, getParentHeight() - 30,
                       85, getParentHeight() - 30,
                       90, pitchWheel.getY()+(pitchWheel.getHeight()/2));
    g.fillPath(arrow2);


    juce::Rectangle<int> transposeArea(7, 10, 40, pitchWheel.getHeight());
    juce::String transpose_(this->transpose);
    g.setFont(22.0f);
    g.drawText(transpose_, transposeArea, juce::Justification::centred);

    juce::Path upArrow;
    juce::Path lowArrow;
    lowArrow.addTriangle(22, 75,
                         32, 75,
                         27, 85);
    upArrow.addTriangle(27, 20,
                        22, 30,
                        32, 30);

    g.fillPath(upArrow);
    g.fillPath(lowArrow);

}

void Wheels::resized() {
    setBounds(0, 20 ,150, getParentHeight() - 20);
}
