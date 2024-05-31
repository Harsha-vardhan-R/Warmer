/*
  ==============================================================================

    Envelope.h
    Created: 12 May 2024 11:27:08am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

class Envelope : public GraphNode {
public:

    typedef void (Envelope::*callbackFunction_type)();

    Envelope(int pos_x, int pos_y) : GraphNode(juce::String("Envelope"), pos_x, pos_y)  {


        OutputSockets.add(new GraphNode::Socket(juce::String("Envelope"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Trigger Type"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("Trigger");
        InputSockets[0]->addMenuItem("Trigger & Hold");
        InputSockets[0]->addMenuItem("Loop");
        InputSockets[0]->addMenuItem("Continue");


        InputSockets.add(new GraphNode::Socket(juce::String("Duration(ms)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(0.0, 2000.0, 25.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Shape"), direction::IN, false));
        InputSockets[2]->addEnvParameterControl();

        makeAllSocketsVisible();
        resized();
    }


    void mini_reset() override {
        callbackFunction_type callBackTempPointer;

        callBackFunctionEnvelope.store(callBackTempPointer);
    }

    void processGraphNode() override {
        callbackFunction_type callback = callBackFunctionEnvelope.load();
        (this->*callBackFunctionEnvelope)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);


    }

    ~Envelope() {};

private:

    std::atomic<callbackFunction_type> callBackFunctionEnvelope = nullptr;
};