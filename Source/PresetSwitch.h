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

    PresetSwitch() {
        instrumentSelect = std::make_unique<juce::ComboBox>();
        instrumentSelect->setJustificationType(juce::Justification::centred);
        instrumentSelect->setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        instrumentSelect->setText("Select Instrument");
        addAndMakeVisible(instrumentSelect.get());

        presetSelect = std::make_unique<juce::ComboBox>();
        presetSelect->setJustificationType(juce::Justification::centred);
        presetSelect->setColour(juce::ComboBox::textColourId, ComboBoxTextSelectedColourID);
        // will change the text when you are changing the instrument.
        presetSelect->setText("Select Preset");
        addAndMakeVisible(presetSelect.get());

        volumeBars = std::make_unique<VolumeLevel>();
        addAndMakeVisible(volumeBars.get());
        VolumeLevel::instance = volumeBars.get();

        waveShape = std::make_unique<WaveShapeDisp>(2);
        addAndMakeVisible(waveShape.get());

        juce::Image myImage = juce::ImageFileFormat::loadFrom(juce::File("./WarmerLogo.png"));
        logo = std::make_unique<juce::ImageComponent>();
        logo->setImage(myImage);
        addAndMakeVisible(logo.get());

        styles = std::make_unique<MenuComponent>();
        setLookAndFeel(styles.get());

        resized();
    };

    /* Call from outside when the instruments are being loaded */
    void addInstrument(const juce::String& instrumentName) {
        instrument_number_count++;
        instrumentSelect->addItem(instrumentName, instrument_number_count);
    }

    ~PresetSwitch() override {
        setLookAndFeel(nullptr);
    };

    void paint(juce::Graphics& g) override {
        g.fillAll(WheelComponentBackgroundID);
    };

    void resized() override {
        setSize(getParentWidth(), 35);

        if (instrumentSelect != nullptr) instrumentSelect->setBounds(130, 1, (getParentWidth()-130)*0.4f, 33);
        if (logo != nullptr) logo->setBounds(10, 2, 115, 31);
        if (presetSelect != nullptr) presetSelect->setBounds(130+instrumentSelect->getWidth(), 1, (getParentWidth()-130)*0.25f, 33);
        if (volumeBars != nullptr)
            volumeBars->setBounds(presetSelect->getX()+presetSelect.get()->getWidth()+1, 2, getWidth()-(367+instrumentSelect->getWidth()+presetSelect->getWidth()), 31);
        if (waveShape != nullptr) waveShape->setBounds(getWidth()-234, 2, 232, 31);

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
