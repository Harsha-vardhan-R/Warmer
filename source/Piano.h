
#pragma once
#include "Wheels.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_audio_utils/juce_audio_utils.h>

class Piano : public juce::MidiKeyboardComponent {
public:

    static Piano* Instance_Pointer;

    Piano(juce::MidiKeyboardState& state, juce::KeyboardComponentBase::Orientation orientation, int n);
    ~Piano() override;

//    void paint(juce::Graphics& g) override;
    void resized() override;

    void setOffset(int n) {
        offset = n;
    }

    void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour noteFillColour) override {
        g.setColour(juce::Colours::grey);

        if (isDown) g.setColour(juce::Colours::darkgrey);

        g.fillRoundedRectangle(area, 1.0f);
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(area, 1.0f, 1.0f);
    }

//    void focusLost(juce::Component::FocusChangeType a) {
//        grabKeyboardFocus();
//    }

    static void setMidiNumToListenTo(int channelNumber) {
        if (Instance_Pointer) Instance_Pointer->setMidiChannel(channelNumber);
    }


private:

    int offset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Piano)
};
