/*
  ==============================================================================

    Oscillator.h
    Created: 27 Mar 2024 4:16:44pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Oscillator : public GraphNode {
public:

    Oscillator(int pos_x, int pos_y) : GraphNode(juce::String("OSCILLATOR"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        // Type of the basic wave shape.
        InputSockets.add(new Socket(juce::String("Wave Shape"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("None");
        InputSockets[0]->addMenuItem("Sine");
        InputSockets[0]->addMenuItem("Square");
        InputSockets[0]->addMenuItem("Triangle");
        InputSockets[0]->addMenuItem("Saw-tooth");

        // frequency.
        InputSockets.add(new Socket(juce::String("Frequency (Hz)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::MIDI);
        InputSockets[1]->addSliderParameterControl(0.0, 20000.0, 150.0); // frequency range.

        // Amplitude.
        InputSockets.add(new Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(0.0, 1.2, 0.5);

        // Phase.
        InputSockets.add(new Socket(juce::String("Phase (deg)"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[3]->addSliderParameterControl(0.0, 360.0, 0.0);

        makeAllSocketsVisible();


        resized();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
        float freq = InputSockets[1]->getValue();
        std::cout << freq << "\n";
    }

    void prepare() override {

    }

    void releaseResources() override {}
    void reset() override {}


    ~Oscillator() {};

private:

    double currentAngle;
    double angleDelta;

    juce::MidiBuffer midiBuffer;

};