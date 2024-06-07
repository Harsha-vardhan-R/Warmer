/*
  ==============================================================================

    PresetSwitch.h
    Created: 12 Feb 2024 11:48:41pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include <memory>
#include "MenuComponent.h"
#include "ColourPalette.h"
#include "VolumeLevel.h"
#include "WaveShapeDisp.h"


/*
    The class that handles selecting various instruments and various presets if the instruments have them available,
    This bar also contains the Final Gain and a Spectrometer,
    Both are inspired from Vital and both will look like nearly same as how Vital has them.
*/
class PresetSwitch : public juce::Component {
public:

    PresetSwitch() : waveShape(2) {
        instrumentSelect.setJustificationType(juce::Justification::centred);
        instrumentSelect.setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        instrumentSelect.setText("Select Instrument");
        addAndMakeVisible(instrumentSelect);


        presetSelect.setJustificationType(juce::Justification::centred);
        presetSelect.setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        // will change the text when you are changing the instrument.
        presetSelect.setText("Select Preset");
        addAndMakeVisible(presetSelect);


        addAndMakeVisible(volumeBars);
        VolumeLevel::instance = &volumeBars;


        addAndMakeVisible(waveShape);

        juce::Image myImage = juce::ImageFileFormat::loadFrom(juce::File("./WarmerLogo.png"));
        logo.setImage(myImage);
        addAndMakeVisible(logo);


        setLookAndFeel(&styles);

        resized();
    };

    /* Call from outside when the instruments are being loaded */
    void addInstrument(const juce::String& instrumentName) {
        instrument_number_count++;
        instrumentSelect.addItem(instrumentName, instrument_number_count);
    }

    ~PresetSwitch() override {
        setLookAndFeel(nullptr);
    };

    void paint(juce::Graphics& g) override {
        g.fillAll(WheelComponentBackgroundID);
    };

    void resized() override {
        setSize(getParentWidth(), 35);

        instrumentSelect.setBounds(130, 1, (getParentWidth()-130)*0.4f, 33);
        logo.setBounds(10, 2, 115, 31);
        presetSelect.setBounds(130+instrumentSelect.getWidth(), 1, (getParentWidth()-130)*0.25f, 33);
        volumeBars.setBounds(presetSelect.getX()+presetSelect.getWidth()+1, 2, getWidth()-(367+instrumentSelect.getWidth()+presetSelect.getWidth()), 31);
        waveShape.setBounds(getWidth()-234, 2, 232, 31);

    };

private:
    int instrument_number_count = 0;

    juce::ImageComponent logo;
    juce::ComboBox instrumentSelect;
    juce::ComboBox presetSelect;
    MenuComponent styles;
    VolumeLevel volumeBars;
    WaveShapeDisp waveShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetSwitch)
};
