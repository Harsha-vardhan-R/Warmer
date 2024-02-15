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

    myCheckbox = std::make_unique<juce::ToggleButton>();
    myCheckbox.get()->setButtonText("Polyphony");
    myCheckbox.get()->setColour(juce::ToggleButton::tickColourId, juce::Colours::green);
    addAndMakeVisible(myCheckbox.get());
    myCheckbox.get()->addListener(this);

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

    myCheckbox.get()->setBounds(width*0.5f, 0, height-2, height-2);
    comboBox.get()->setBounds(width*0.5f + 225, 1, width*0.1f, height-2);
}

void MIDISettings::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::black);
}