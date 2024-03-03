/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 3 Mar 2024 3:28:07pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MyLookAndFeel.h"
#include "ColourPalette.h"

void MyLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) {

    g.setColour(ButtonOutlineColourID);
    g.drawRoundedRectangle(g.getClipBounds().toFloat(), 4.0, 2);
}

void MyLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                 int buttonW, int buttonH, juce::ComboBox& box) {
    g.setColour(MenuMouseOverColourID);
    g.fillRoundedRectangle(box.getLocalBounds().reduced(2).toFloat(), 3.0f);

    juce::Path arrow;
    float arrowDimen = 5;
    float arrowX = width - 15; // X position of the arrow
    float arrowY = (height - arrowDimen) / 2; // Y position of the arrow

    if (box.isPopupActive()) {
        arrow.addTriangle(arrowX + (arrowDimen * 0.5f) + 1, arrowY,
                          arrowX, arrowY + arrowDimen,
                          arrowX + arrowDimen + 2, arrowY + arrowDimen);
    } else {
        arrow.addTriangle(arrowX, arrowY,
                          arrowX + arrowDimen + 2, arrowY,
                          arrowX + (arrowDimen * 0.5f) + 1, arrowY + arrowDimen);
    }

    g.setColour(ComboBoxArrowColourID);
    g.fillPath(arrow);
}

void MyLookAndFeel::drawMenuBarItem (juce::Graphics& g,
                                     int width,
                                     int height,
                                     int itemIndex,
                                     const juce::String& itemText,
                                     bool isMouseOverItem,
                                     bool isMenuOpen,
                                     bool isMouseOverBar,
                                     juce::MenuBarComponent& menuBar) {
    juce::Rectangle<int> highLight(1, 1, width-2, height-2);
    if (isMouseOverItem) {
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
        g.setColour(SelectedTabTextColourID);
    } else {
        g.setColour(IdleTabTextColourID);
    }
    g.drawText(itemText, 0, 0, width, height, juce::Justification::centred);
}