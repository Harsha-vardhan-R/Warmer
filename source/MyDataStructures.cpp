/*
  ==============================================================================

    MyDataStructures.cpp
    Created: 15 May 2024 10:08:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MyDataStructures.h"
#include "Instrument.h"
#include "juce_audio_basics/juce_audio_basics.h"

void TopoSorter::setNextMidiBuffer(juce::MidiBuffer& buffer) {
    Instrument* pointerToInstrument = Instrument::getInstance();

    if (pointerToInstrument) {
        midiBuffer = buffer;
        pointerToInstrument->keyboardState.processNextMidiBuffer(midiBuffer, 0, sampleSize, true);
    }

}
