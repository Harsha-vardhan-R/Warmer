/*
  ==============================================================================

    MIDI_Transpose.h
    Created: 12 May 2024 2:40:52pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Transpose : public GraphNode {
public:

    Transpose(int pos_x, int pos_y) : GraphNode(juce::String("Transpose"), pos_x, pos_y) {

        OutputSockets.add(new GraphNode::Socket("Transposed", direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("MIDI"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("Transpose Amount"), direction::IN, false));
        InputSockets[1]->addMenuParameterControl();
        InputSockets[1]->addMenuItem("-5");
        InputSockets[1]->addMenuItem("-4");
        InputSockets[1]->addMenuItem("-3");
        InputSockets[1]->addMenuItem("-2");
        InputSockets[1]->addMenuItem("-1");
        InputSockets[1]->addMenuItem("0");
        InputSockets[1]->addMenuItem("1");
        InputSockets[1]->addMenuItem("2");
        InputSockets[1]->addMenuItem("3");
        InputSockets[1]->addMenuItem("4");
        InputSockets[1]->addMenuItem("5");


        makeAllSocketsVisible();
        resized();

        InputSockets[1]->selectMenuItem(6);

    }

    void processGraphNode() override {
        int val = (int)InputSockets[1]->getValue();

        juce::MidiMessage message;

        midiBufferOut.clear();

        // Iterate through all the messages in the buffer
        for (const auto metadata : *readBuff) {
            int samplePosition = metadata.samplePosition;
            message = metadata.getMessage();

            juce::MidiMessage newMessage(message, samplePosition);
            newMessage.setNoteNumber(std::clamp(message.getNoteNumber() + (val - 6) * 12 , 0, 127));

            midiBufferOut.addEvent(newMessage, samplePosition);
        }

    }

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getMidiBuffer();
        OutputSockets[0]->setMidiBufferPointer(&midiBufferOut);
    }


private:

    juce::MidiBuffer* readBuff;

    juce::MidiBuffer midiBufferOut;
};


class Velocity : public GraphNode {
public:

    Velocity(int pos_x, int pos_y) : GraphNode(juce::String("Velocity"), pos_x, pos_y) {

        OutputSockets.add(new GraphNode::Socket("MIDI", direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        OutputSockets.add(new GraphNode::Socket("Velocity", direction::OUT, false));
        OutputSockets[1]->setOutputType(SocketDataType::Floating);

        InputSockets.add(new GraphNode::Socket(juce::String("MIDI"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::MIDI);

        makeAllSocketsVisible();
        resized();
    }

    void processGraphNode() override {
        juce::MidiMessage message;

        midiBufferOut.clear();

        // Iterate through all the messages in the buffer
        for (const auto metadata : *readBuff) {
            int samplePosition = metadata.samplePosition;
            message = metadata.getMessage();

            juce::MidiMessage newMessage(message, samplePosition);
            midiBufferOut.addEvent(newMessage, samplePosition);

            OutputSockets[1]->setFloatValue(message.getFloatVelocity());
        }

    }

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getMidiBuffer();
        OutputSockets[0]->setMidiBufferPointer(&midiBufferOut);
    }


private:
    juce::MidiBuffer* readBuff;

    juce::MidiBuffer midiBufferOut;
};

class Invert : public GraphNode {
public:

    Invert(int pos_x, int pos_y) : GraphNode(juce::String("Invert"), pos_x, pos_y) {

        OutputSockets.add(new GraphNode::Socket("MIDI", direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("MIDI"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::MIDI);

        makeAllSocketsVisible();
        resized();
    }

    void processGraphNode() override {
        juce::MidiMessage message;

        midiBufferOut.clear();

        // Iterate through all the messages in the buffer
        for (const auto metadata : *readBuff) {
            int samplePosition = metadata.samplePosition;
            message = metadata.getMessage();
            int note = message.getNoteNumber();

            if (message.isNoteOff()) {
                juce::MidiMessage newMessage(juce::MidiMessage::noteOn(message.getChannel(), note, message.getFloatVelocity()));
                midiBufferOut.addEvent(newMessage, samplePosition);
            } else if (message.isNoteOn()) {
                juce::MidiMessage newMessage(juce::MidiMessage::noteOff(message.getChannel(), note, message.getFloatVelocity()));
                midiBufferOut.addEvent(newMessage, samplePosition);
            }
        }

    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setMidiBufferPointer(&midiBufferOut);
        readBuff = InputSockets[0]->getMidiBuffer();
    }


private:
    juce::MidiBuffer* readBuff;

    juce::MidiBuffer midiBufferOut;
};