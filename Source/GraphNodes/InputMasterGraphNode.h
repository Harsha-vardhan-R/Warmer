/*
  ==============================================================================

    InputMasterGraphNode.h
    Created: 4 Mar 2024 9:59:02pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"

class InputMasterGraphNode : public GraphNode {
public:

    InputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("MIDI IN",
                      pos_x,
                      pos_y) {

        needAudioBuffer = false;

        OutputSockets.add(new Socket(juce::String("MIDI"), direction::OUT, false));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        OutputSockets.add(new Socket(juce::String("Pitch Wheel"), direction::OUT, false));
        OutputSockets[1]->setOutputType(SocketDataType::Floating);

        OutputSockets.add(new Socket(juce::String("Mod Wheel"), direction::OUT, false));
        OutputSockets[2]->setOutputType(SocketDataType::Floating);

        canBeDeleted = false;
        makeAllSocketsVisible();
        resized();
    };

    // callback functions.
    template < bool processWheels>
    void subProcessCallback() {

        if constexpr (processWheels) {

            // Iterate through all the messages in the buffer
            for (const auto metadata : *readBuff) {
                const auto& message = metadata.getMessage();

                if (message.isPitchWheel()) {
                    pitchValue = ((float)(message.getPitchWheelValue() - 8192)) / 8192.0f;
                } else if (message.isController() && message.getControllerNumber() == 1) {
                    modWheelValue = message.getControllerValue();
                }

            }

            OutputSockets[1]->setFloatValue(pitchValue);
            OutputSockets[2]->setFloatValue(((float)modWheelValue)/127.0);

        } else {
            /* Do Literally Nothing */
        }
    }

    void processGraphNode() override {
        (this->*callBackFunction)();
    }

    void setInputMIDI(juce::MidiBuffer* buffer) {
        readBuff = buffer;
    }

    void releaseResources() override {}
    void reset() override {
        OutputSockets[0]->setMidiBufferPointer(readBuff);

        pitchValue = 0; modWheelValue = 0;

        if (OutputSockets[1]->isThisConnected() || OutputSockets[2]->isThisConnected()) {
            callBackFunction = &InputMasterGraphNode::subProcessCallback<true>;
        } else {
            callBackFunction = &InputMasterGraphNode::subProcessCallback<false>;
        }

    }

    ~InputMasterGraphNode() {};

private:

    juce::MidiBuffer* readBuff;

    void (InputMasterGraphNode::*callBackFunction)() = nullptr;

    float pitchValue = 0.0; int modWheelValue = 0;

};