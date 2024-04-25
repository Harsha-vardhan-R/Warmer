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
    AudioBufferFloat,
    AudioBufferFloatSingle, // a single channel audio buffer created by things such as ADSR etc...
    Floating,
    NULLType, // when you cannot modulate with any other output.
};

enum direction {
    IN,
    OUT
};