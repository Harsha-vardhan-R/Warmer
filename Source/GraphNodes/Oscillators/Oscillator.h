/*
  ==============================================================================

    Oscillator.h
    Created: 27 Mar 2024 4:16:44pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../GraphNode.h"

class Oscillator : public GraphNode {
public:

    Oscillator(int pos_x, int pos_y) : GraphNode(juce::String("OSCILLATOR"), pos_x, pos_y) {

        // Type of the basic wave shape.
        InputSockets.add(new Socket(juce::String("Wave Shape"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("SINE");
        InputSockets[0]->addMenuItem("SQUARE");
        InputSockets[0]->addMenuItem("TRIANGLE");
        InputSockets[0]->addMenuItem("SAW");

        // frequency.
        InputSockets.add(new Socket(juce::String("Frequency"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::MIDI);
        InputSockets[1]->addSliderParameterControl(0.0, 20000.0, 300.0); // frequency range.

        // Amplitude.
        InputSockets.add(new Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);

        // Phase.
        InputSockets.add(new Socket(juce::String("Phase"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->acceptType(SocketDataType::AudioBufferFloat);

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        resized();
    }

    void process() override {
//        float waveType = InputSockets[0]->get
    }

    void update(double bitRate, int bufferSize) override {

    }

    ~Oscillator() override {};

private:


};