/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 3 Mar 2024 3:28:07pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MyLookAndFeel : public juce::LookAndFeel_V4 {
public:


    void drawTextEditorOutline(juce::Graphics& g,
                               int width,
                               int height,
                               juce::TextEditor& textEditor) override;

    void drawComboBox ( juce::Graphics& g,
                        int width,
                        int height,
                        bool isButtonDown,
                        int buttonX,
                        int buttonY,
                        int buttonW,
                        int buttonH,
                        juce::ComboBox& box) override;

    void drawMenuBarItem (juce::Graphics& g,
                                         int width,
                                         int height,
                                         int itemIndex,
                                         const juce::String& itemText,
                                         bool isMouseOverItem,
                                         bool isMenuOpen,
                                         bool isMouseOverBar,
                                         juce::MenuBarComponent& menuBar) override;
private:

};