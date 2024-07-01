#pragma once

#include "Instrument.h"
#include "PluginProcessor.h"
#include "MainComponent.h"

// #include "BinaryData.h"
// #include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================

class PluginEditor : public juce::AudioProcessorEditor {
public:

    explicit PluginEditor (MainComponent* ,PluginProcessor&);
    ~PluginEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    MainComponent* mainCompRef = nullptr;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
