#include "PluginEditor.h"
#include "Instrument.h"
#include "MainComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"
PluginEditor::PluginEditor (MainComponent* m, PluginProcessor& p) : AudioProcessorEditor (&p), 
    processorRef (p) {

    mainCompRef = m;

    // The main component from Warmer.
    addAndMakeVisible(mainCompRef);

    setSize (1700, 900);

  setResizable(false, false);

    setResizeLimits(1300, 750, 3000, 5000);
}

PluginEditor::~PluginEditor() {}

// No Painting cause this is the last layer of GUI,
// MainComponent will completely lie over this.
void PluginEditor::paint (juce::Graphics& g) {}

void PluginEditor::resized() { if (mainCompRef) mainCompRef->setSize(getWidth(), getHeight()); }
