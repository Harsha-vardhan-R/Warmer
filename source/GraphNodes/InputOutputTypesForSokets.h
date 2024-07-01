/*
  ==============================================================================

    InputOutputTypesForSokets.h
    Created: 23 Feb 2024 9:48:03pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <iostream>

// Types of an input you can send into the input of a node.

// MIDI is a juce::MidiMessage,
// AudioBufferFloat is juce::AudioBuffer<float> for continuous values,
// Floating is a float point that is read once for processing each buffer.

enum class SocketDataType {
    MIDI,
    AudioBufferFloat,
    Floating,
    NULLType, // when you cannot modulate with any other output.
};

std::ostream& operator<<(std::ostream& os, SocketDataType e);

enum class direction {
    IN,
    OUT
};
