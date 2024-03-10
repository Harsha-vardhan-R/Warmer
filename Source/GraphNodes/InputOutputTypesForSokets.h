/*
  ==============================================================================

    InputOutputTypesForSokets.h
    Created: 23 Feb 2024 9:48:03pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


// Types of an input you can send into the input of a node.
enum SocketDataType {
    MIDI,
    AudioBufferInt,
    AudioBufferFloat,
    Integer,
    Floating,
    NULLType, // when there exists no node like this.
};

enum direction {
    IN,
    OUT
};