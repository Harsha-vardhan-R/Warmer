/*
  ==============================================================================

    SampleSound.h
    Created: 2 Apr 2024 5:52:58am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once


/*
  ==============================================================================

    Oscillator.h
    Created: 27 Mar 2024 4:16:44pm
    Author:  harshavardhan

  ==============================================================================
*/


// MOCK UP
#pragma once
#include <JuceHeader.h>
#include "../GraphNode.h"

class SampleSound : public GraphNode {
public:

    SampleSound(int pos_x, int pos_y) : GraphNode(juce::String("SAMPLE SOUND"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Volume"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        OutputSockets.add(new Socket(juce::String("Peak Freq"), direction::OUT, true));
        OutputSockets[1]->setOutputType(SocketDataType::AudioBufferFloat);

        OutputSockets.add(new Socket(juce::String("Stereo Diff"), direction::OUT, true));
        OutputSockets[2]->setOutputType(SocketDataType::AudioBufferFloat);

//        // Type of the basic wave shape.
//        InputSockets.add(new Socket(juce::String("Wave Shape"), direction::IN, false));
//        InputSockets[0]->addMenuParameterControl();
//        InputSockets[0]->addMenuItem("SINE");
//        InputSockets[0]->addMenuItem("SQUARE");
//        InputSockets[0]->addMenuItem("TRIANGLE");
//        InputSockets[0]->addMenuItem("SAW");

        // frequency.
        InputSockets.add(new Socket(juce::String("Start"), direction::IN, false));
        InputSockets[0]->acceptType(SocketDataType::MIDI);
        InputSockets[0]->addSliderParameterControl(0.0, 20000.0, 300.0); // frequency range.

        // Amplitude.
        InputSockets.add(new Socket(juce::String("End"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 20000.0, 300.0);

//        // Phase.
        InputSockets.add(new Socket(juce::String("File Path"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(0.0, 360.0, 0.0);

        makeAllSocketsVisible();
        resized();
    }

    void process() override {
//        float waveType = InputSockets[0]->get
    }

    void update(double bitRate, int bufferSize) override {

    }

    ~SampleSound() {};

private:


};