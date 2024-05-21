/*
  ==============================================================================

    PresetSwitch.h
    Created: 12 Feb 2024 11:48:41pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
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

    PresetSwitch() {
        instrumentSelect = std::make_unique<juce::ComboBox>();
        instrumentSelect.get()->setJustificationType(juce::Justification::centred);
        instrumentSelect.get()->setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        instrumentSelect.get()->setText("Select Instrument");
        addAndMakeVisible(instrumentSelect.get());

        presetSelect = std::make_unique<juce::ComboBox>();
        presetSelect.get()->setJustificationType(juce::Justification::centred);
        presetSelect.get()->setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        // will change the text when you are changing the instrument.
        presetSelect.get()->setText("Select Preset");
        addAndMakeVisible(presetSelect.get());

        volumeBars.reset(new VolumeLevel());
        addAndMakeVisible(volumeBars.get());
        VolumeLevel::instance = volumeBars.get();

        waveShape.reset(new WaveShapeDisp(2));
        addAndMakeVisible(waveShape.get());

        juce::Image myImage = juce::ImageFileFormat::loadFrom(juce::File("./Assets/Icons/icon.png"));
        logo = std::make_unique<juce::ImageComponent>();
        logo.get()->setImage(myImage);
        addAndMakeVisible(logo.get());

        styles.reset(new MenuComponent());
        setLookAndFeel(styles.get());

        resized();
    };

    /* Call from outside when the instruments are being loaded */
    void addInstrument(juce::String instrumentName) {
        instrument_number_count++;
        instrumentSelect.get()->addItem(instrumentName, instrument_number_count);
    }

    ~PresetSwitch() override {
        setLookAndFeel(nullptr);
    };

    void paint(juce::Graphics& g) override {
        g.fillAll(WheelComponentBackgroundID);
    };

    void resized() override {
        setSize(getParentWidth(), 35);

        if (instrumentSelect.get() != nullptr) instrumentSelect.get()->setBounds(130, 1, (getParentWidth()-130)*0.4f, 33);
        if (logo.get() != nullptr) logo.get()->setBounds(10, 2, 115, 31);
        if (presetSelect.get() != nullptr) presetSelect.get()->setBounds(130+instrumentSelect.get()->getWidth(), 1, (getParentWidth()-130)*0.25f, 33);
        if (volumeBars.get() != nullptr) volumeBars.get()->setBounds(presetSelect.get()->getX()+presetSelect.get()->getWidth()+1, 2, getWidth()-(367+instrumentSelect.get()->getWidth()+presetSelect.get()->getWidth()), 31);
        if (waveShape.get() != nullptr) waveShape.get()->setBounds(getWidth()-234, 2, 232, 31);

    };

private:
    int instrument_number_count = 0;

    std::unique_ptr<juce::ImageComponent> logo;
    std::unique_ptr<juce::ComboBox> instrumentSelect;
    std::unique_ptr<juce::ComboBox> presetSelect;
    std::unique_ptr<juce::LookAndFeel_V4> styles;
    std::unique_ptr<VolumeLevel> volumeBars;
    std::unique_ptr<WaveShapeDisp> waveShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetSwitch)
};
