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
    Integer, // for discrete types in selecting the type of something in a `TypeList`.
    Floating,
    FloatingVector, // for smooth transitions, is the output from the user
    NULLType, // when you cannot modulate with any other output.
};

enum direction {
    IN,
    OUT
};