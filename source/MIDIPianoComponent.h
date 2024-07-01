/*
  ==============================================================================

    MIDIPianoComponent.h
    Created: 14 Feb 2024 5:29:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Piano.h"
#include "Wheels.h"
#include "PianoSideBar.h"


// TODO : Command listener so that we can use keyboard for midi input without velocity.
class MIDIPianoComponent : public juce::Component {
public:

    MIDIPianoComponent(int offset, int height);
    ~MIDIPianoComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int offset;
    int height;

    Wheels wheelComponent;
    Piano pianoComponent;
    PianoSideBar sideBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIPianoComponent)
};
