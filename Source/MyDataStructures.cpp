/*
  ==============================================================================

    MyDataStructures.cpp
    Created: 15 May 2024 10:08:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "MyDataStructures.h"
#include "Instrument.h"

void PriorityQueue::setNextMidiBuffer() {
    Instrument* pointerToInstrument = Instrument::getInstance();

    if (pointerToInstrument) {
        pointerToInstrument->removeNextBlockOfMessages(midiBuffer, sampleSize);
    }

}