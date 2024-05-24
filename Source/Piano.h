/*
  ==============================================================================

    Piano.h
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Wheels.h"


class Piano : public juce::MidiKeyboardComponent {
public:

    static Piano* Instance_Pointer;

    Piano(juce::MidiKeyboardState& state, juce::KeyboardComponentBase::Orientation orientation);
    ~Piano() override;

//    void paint(juce::Graphics& g) override;
    void resized() override;

    void setOffset(int n) {
        offset = n;
    }

    void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour noteFillColour) override {

        g.setColour(juce::Colours::grey);

        if (isDown) {
            g.setColour(juce::Colours::grey);
            g.fillRoundedRectangle(area, 1.0f);
            return;
        }

        g.fillRoundedRectangle(area, 1.0f);

        g.setColour(juce::Colours::darkgrey);

        g.drawRoundedRectangle(area, 1.0f, 1.0f);
    }

//    void focusLost(juce::Component::FocusChangeType a) {
//        grabKeyboardFocus();
//    }

    static void setMidiNumToListenTo(int channelNumber) {
        Piano::Instance_Pointer->setMidiChannel(channelNumber);
    }


private:

    int offset;

    const int blackKeysPosition[37] = { 1,  2,  4,  5,  6,
                                        8,  9, 11, 12, 13,
                                       15, 16, 18, 19, 20,
                                       22, 23, 25, 26, 27,
                                       29, 30, 32, 33, 34,
                                       36, 37, 39, 40, 41,
                                       43, 44, 46, 47, 48,
                                       50, 51};

    const int keyColour[87] = {
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1
    };



    juce::Image PianoImage = juce::ImageFileFormat::loadFrom(juce::File("./Assets/KeyPressedImages/piano_full_unpressed.png"));


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Piano)
};