/*
  ==============================================================================

    MIDIPianoComponent.h
    Created: 14 Feb 2024 5:29:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MIDISettings.h"
#include "Piano.h"
#include "Wheels.h"

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

    std::unique_ptr<MIDISettings> midiSettings;
    std::unique_ptr<Wheels> wheelComponent;
    std::unique_ptr<Piano> pianoComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIPianoComponent)
};