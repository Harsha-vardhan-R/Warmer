/*
  ==============================================================================

    Mixer.h
    Created: 27 May 2024 1:37:39pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <iostream>

class Mixer : public GraphNode {
public:

    Mixer(int pos_x, int pos_y) : GraphNode(juce::String("Mixer"), pos_x, pos_y) {

        InputSockets.add(new GraphNode::Socket(juce::String("No. of Channels"), direction::IN, true));
        InputSockets[0]->addMenuParameterControl();
        for (int i = 0; i < 17; ++i) InputSockets[0]->addMenuItem(juce::String(i));

        makeAllSocketsVisible();
        resized();
    }

    void processGraphNode() override {

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) channelData[i] = 0.0f;
            for (int voice = 0; voice < voices; ++voice) {
                float mixAmount = InputSockets[voice]->getValue();
                const float* inputChannel = audioBufferPointerArray[voice]->getReadPointer(channel);

                for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                    channelData[i] += inputChannel[i]*mixAmount;
                }
            }
        }

    }

    void releaseResources() override {}

    void reset() override {
        if (init_phase_done) {
            OutputSockets[0]->setBufferPointer(bufferToWritePointer);
            for (int i = 0; i < voices; ++i) {
                audioBufferPointerArray[i] = InputSockets[i]->getBufferPointer();
            }
        }
    }

    void mini_reset() override {

        if (!init_phase_done) {
            int voices = InputSockets[0]->getValue()-1;

            // std::cout << "Called the mini-reset here" << "\n";

            InputSockets.clear();
            OutputSockets.clear();

            for (int i = 0; i < voices; ++i) {
                juce::String temp(i+1);
                InputSockets.add(new GraphNode::Socket(juce::String("Channel "+temp), direction::IN, true));
                InputSockets[i]->acceptType(SocketDataType::AudioBufferFloat);
                InputSockets[i]->addSliderParameterControl(0.0f, 1.0f, 1.0f/(float)voices);
                InputSockets[i]->setParameterCtrlNoCollapseOnConnection();
            }

            OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
            OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

            audioBufferPointerArray.resize(voices);

            init_phase_done = true;
            this->voices = voices;

            makeAllSocketsVisible();
            resized();

        }

    }

    void mini_makeXML(juce::XmlElement *x) override {
            // This cannot be -1 as we cannot save 
            // if all the nodes are not connected and 
            x->setAttribute("meta", voices);
    }

    void mini_parseXMLChildren(juce::XmlElement *x) override {
        int vo = x->getIntAttribute("meta");
        // Now set that many voices.
        InputSockets[0]->setParamValue(vo+1.0f);
        // std::cout << "Called the parseChildren" << "\n";
    }

    ~Mixer() {};

private:

    bool init_phase_done = false;

    std::vector<juce::AudioBuffer<float>*> audioBufferPointerArray;

    int number_of_voices = -1;

    int voices = -1;
};
