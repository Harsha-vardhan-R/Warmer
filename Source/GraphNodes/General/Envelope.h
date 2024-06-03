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
        InputSockets[0]->addMenuItem("Loop Continue");


        InputSockets.add(new GraphNode::Socket(juce::String("Trigger"), direction::IN, true));
        InputSockets[1]->acceptType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("Duration(ms)"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0, 2000.0, 120.0);

        InputSockets.add(new GraphNode::Socket(juce::String("      Envelope   "), direction::IN, false));
        InputSockets[3]->addEnvParameterControl();

        makeAllSocketsVisible();
        resized();

        envelope = static_cast<envParamCtrl*>(InputSockets[3]->getPointerToPresentParameterCtrlController());

        if (!envelope) std::cout << "Dayum, Did not work out!" << "\n";
    }

    template<int type>
    void subProcessGenerator() {


        if (type == 1) {

        }
    }


    void mini_reset() override {
//        callbackFunction_type callBackTempPointer;
//
//        callBackFunctionEnvelope.store(callBackTempPointer);
    }

    void processGraphNode() override {
//        callbackFunction_type callback = callBackFunctionEnvelope.load();
//        (this->*callBackFunctionEnvelope)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

    }

    ~Envelope() {};

private:
    std::atomic<callbackFunction_type> callBackFunctionEnvelope = nullptr;

    envParamCtrl* envelope = nullptr;


    // time from 0 -> 1
    // we just
    float timeCoordinate = 0.0f;
};