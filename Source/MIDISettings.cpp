/*
  ==============================================================================

    MIDISettings.cpp
    Created: 15 Feb 2024 7:22:37am
    Author:  harshavardhan

  ==============================================================================
*/

#include "MIDISettings.h"
#include "MenuComponent.h"

MIDISettings::MIDISettings() {

    myCheckbox = std::make_unique<juce::TextButton>();
    myCheckbox.get()->setClickingTogglesState(true);
    myCheckbox.get()->setButtonText("Polyphony");
    myCheckbox.get()->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::pink);
    myCheckbox.get()->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    myCheckbox.get()->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::darkgrey);
    addAndMakeVisible(myCheckbox.get());
    myCheckbox.get()->changeWidthToFitText();
    myCheckbox.get()->addListener(this);

    MIDIFromKeyboard = std::make_unique<juce::TextButton>();
    MIDIFromKeyboard.get()->setClickingTogglesState(true);
    MIDIFromKeyboard.get()->setButtonText("Keyboard->MIDI");
    MIDIFromKeyboard.get()->changeWidthToFitText();
    MIDIFromKeyboard.get()->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::pink);
    MIDIFromKeyboard.get()->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    MIDIFromKeyboard.get()->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::darkgrey);
    addAndMakeVisible(MIDIFromKeyboard.get());
    MIDIFromKeyboard.get()->addListener(this);

    comboBox = std::make_unique<juce::ComboBox>();
    comboBox.get()->addItem("2", 2); comboBox.get()->addItem("3", 3);
    comboBox.get()->addItem("4", 4); comboBox.get()->addItem("5", 5);
    comboBox.get()->addItem("6", 6); comboBox.get()->addItem("7", 7);
    comboBox.get()->addItem("8", 8); comboBox.get()->addItem("9", 9);
    comboBox.get()->addItem("10", 10); comboBox.get()->addItem("11", 11);
    comboBox.get()->addItem("12", 12); comboBox.get()->addItem("13", 13);
    comboBox.get()->addItem("14", 14); comboBox.get()->addItem("15", 15);
    comboBox.get()->addItem("16", 16);
    comboBox.get()->setSelectedId(8);
    addAndMakeVisible(comboBox.get());
    comboBox.get()->addListener(this);

    styles.reset(new MenuComponent());
    setLookAndFeel(styles.get());

    resized();
}

MIDISettings::~MIDISettings() {}

void MIDISettings::buttonClicked(juce::Button* b) {}

void MIDISettings::comboBoxChanged(juce::ComboBox*  comboBoxThatHasChanged) {}

void MIDISettings::resized() {
    setSize(getParentWidth(), 20);

    int width = getWidth();
    int height = getHeight();

    MIDIFromKeyboard.get()->setBounds(300, 1, 55, height-2);
    myCheckbox.get()->setSize(65, height-2);
    myCheckbox.get()->setBounds(15, 1, 65, height-2);
    comboBox.get()->setBounds(myCheckbox.get()->getWidth()+15, 1, 55, height-2);
}

void MIDISettings::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xffeee3e7));
}