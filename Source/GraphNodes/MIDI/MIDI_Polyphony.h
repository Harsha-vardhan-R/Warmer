/*
  ==============================================================================

    MIDI_Polyphony.h
    Created: 12 May 2024 1:14:21pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Polyphony : public GraphNode {
public:

    Polyphony(int pos_x, int pos_y) : GraphNode(juce::String("Polyphony"), pos_x, pos_y) {

        InputSockets.add(new GraphNode::Socket(juce::String("voices"), direction::IN, true));
        InputSockets[0]->addMenuParameterControl();
        for (int i = 0; i < 17; ++i) InputSockets[0]->addMenuItem(juce::String(i));

        makeAllSocketsVisible();
        resized();
    }

    enum class steal_type {
        Revolver, // circular, does not care if non-consecutive voice is empty.
        Latest, // oldest gets replaced.
        Stack
    };

    std::unordered_map<int, int> noteToOutSocketMap;
    std::unordered_map<int, int> SocketToNoteMap;
    std::unordered_set<int> presently_empty;

    std::set<int> presently_empty_order;

    std::queue<int> FIFO_queue;

//    template<steal_type type>
//    void subProcessGenerator() {
//
//        // clear all buffers
//        for (int i = 0; i < voices; ++i) midiBufferArray[i].clear();
//
//        juce::MidiMessage message;
//
//        // Iterate through all the messages in the buffer
//        for (const auto metadata : *readBuff) {
//            message = metadata.getMessage();
//            int samplePosition = metadata.samplePosition;
//            int note = message.getNoteNumber();
//
//            if constexpr (type == steal_type::Latest) {
//
//                if (message.isNoteOn()) {
//
//                    int socket_here = 0;
//
//                    // if there is any free voice we fill it first without stealing another voice.
//                    if (!presently_empty.empty()) {
//                        // take the first socket available here.
//                        socket_first = *presently_empty.begin();
//                        presently_empty.erase(socket_first);
//                    } else if (!FIFO_queue.empty()) { // if it is not empty we steal from the oldest note(the condition should never be false if above one is false in reality).
//                        socket_first = FIFO_queue.first();
//                        FIFO_queue.pop();
//
//                        int position = 0;
//
//                        if (samplePosition == 0) samplePosition++;
//                        else position = samplePosition-1;
//
//                        midiBufferArray[socket_first].addEvent(juce::MidiMessage::noteOff(1, SocketToNoteMap[socket_first], 0), position);
//                    }
//
//                    noteToOutSocketMap[note] = socket_first;
//                    SocketToNoteMap[socket_first] = note;
//                    FIFO_queue.push(socket_first);
//
//                    juce::MidiMessage create_message(message, samplePosition);
//                    midiBufferArray[socket_first].addEvent(create_message, samplePosition);
//                } else if (message.isNoteOff()) {
//                    // if we have the node on, we send a noteOff event.
//                    // the opposing can happen in two ways, stray midi off message,
//                    // where there is no on, or the note is released after being stolen from another.
//                    if (noteToOutSocketMap.count(note)) {
//                        presently_empty.insert(noteToOutSocketMap[note]);
//
//                        juce::MidiMessage create_message(message, samplePosition);
//                        midiBufferArray[noteToOutSocketMap[note]].addEvent(create_message, samplePosition);
//
//                        SocketToNoteMap.erase(noteToOutSocketMap[note]);
//                        noteToOutSocketMap.erase(note);
//                    }
//                }
//
//            } else if (type == steal_type::Stack) {
//                if (message.isNoteOn()) {
//
//                    int socket_here = 0;
//
//                    // if there is any free voice we fill it first without stealing another voice.
//                    if (!presently_empty.empty()) {
//                        // take the first socket available here.
//                        socket_first = *presently_empty.begin();
//                        presently_empty.erase(socket_first);
//                    } else if (!FIFO_queue.empty()) { // if it is not empty we steal from the oldest note(the condition should never be false if above one is false in reality).
//                        socket_first = FIFO_queue.first();
//                        FIFO_queue.pop();
//
//                        int position = 0;
//
//                        if (samplePosition == 0) samplePosition++;
//                        else position = samplePosition-1;
//
//                        midiBufferArray[socket_first].addEvent(juce::MidiMessage::noteOff(1, SocketToNoteMap[socket_first], 0), position);
//                    }
//
//                    noteToOutSocketMap[note] = socket_first;
//                    SocketToNoteMap[socket_first] = note;
//                    FIFO_queue.push(socket_first);
//
//                    juce::MidiMessage create_message(message, samplePosition);
//                    midiBufferArray[socket_first].addEvent(create_message, samplePosition);
//                } else if (message.isNoteOff()) {
//                    // if we have the node on, we send a noteOff event.
//                    // the opposing can happen in two ways, stray midi off message,
//                    // where there is no on, or the note is released after being stolen from another.
//                    if (noteToOutSocketMap.count(note)) {
//                        presently_empty.insert(noteToOutSocketMap[note]);
//
//                        juce::MidiMessage create_message(message, samplePosition);
//                        midiBufferArray[noteToOutSocketMap[note]].addEvent(create_message, samplePosition);
//
//                        SocketToNoteMap.erase(noteToOutSocketMap[note]);
//                        noteToOutSocketMap.erase(note);
//                    }
//                }
//            } else {
//                if (message.isNoteOn()) {
//
//                    int socket_here = 0;
//
//                    // if there is any free voice we fill it first without stealing another voice.
//                    if (!presently_empty.empty()) {
//                        // take the first socket available here.
//                        socket_first = *presently_empty.begin();
//                        presently_empty.erase(socket_first);
//                    } else if (!FIFO_queue.empty()) { // if it is not empty we steal from the oldest note(the condition should never be false if above one is false in reality).
//                        socket_first = FIFO_queue.first();
//                        FIFO_queue.pop();
//
//                        int position = 0;
//
//                        if (samplePosition == 0) samplePosition++;
//                        else position = samplePosition-1;
//
//                        midiBufferArray[socket_first].addEvent(juce::MidiMessage::noteOff(1, SocketToNoteMap[socket_first], 0), position);
//                    }
//
//                    noteToOutSocketMap[note] = socket_first;
//                    SocketToNoteMap[socket_first] = note;
//                    FIFO_queue.push(socket_first);
//
//                    juce::MidiMessage create_message(message, samplePosition);
//                    midiBufferArray[socket_first].addEvent(create_message, samplePosition);
//                } else if (message.isNoteOff()) {
//                    // if we have the node on, we send a noteOff event.
//                    // the opposing can happen in two ways, stray midi off message,
//                    // where there is no on, or the note is released after being stolen from another.
//                    if (noteToOutSocketMap.count(note)) {
//                        presently_empty.insert(noteToOutSocketMap[note]);
//
//                        juce::MidiMessage create_message(message, samplePosition);
//                        midiBufferArray[noteToOutSocketMap[note]].addEvent(create_message, samplePosition);
//
//                        SocketToNoteMap.erase(noteToOutSocketMap[note]);
//                        noteToOutSocketMap.erase(note);
//                    }
//                }
//            }
//
//
//        }
//
//
//    }

    void processGraphNode() override {
        // clear all buffers
        for (int i = 0; i < voices; ++i) midiBufferArray[i].clear();

        juce::MidiMessage message;

        // Iterate through all the messages in the buffer
        for (const auto metadata : *readBuff) {
            message = metadata.getMessage();
            int samplePosition = metadata.samplePosition;
            int note = message.getNoteNumber();

            if (message.isNoteOn() && !FIFO_queue.empty()) {
                int socket_here = FIFO_queue.front();
                FIFO_queue.pop();

                int position = 0;

                if (samplePosition == 0) samplePosition++;
                else position = samplePosition-1;

                midiBufferArray[socket_here].addEvent(juce::MidiMessage::noteOff((int)1, (int)SocketToNoteMap[socket_here], (juce::uint8)0), position);

                noteToOutSocketMap[note] = socket_here;
                SocketToNoteMap[socket_here] = note;
                FIFO_queue.push(socket_here);

                juce::MidiMessage create_message(message, samplePosition);
                midiBufferArray[socket_here].addEvent(create_message, samplePosition);
            } else if (message.isNoteOff()) {
                // if we have the node on, we send a noteOff event.
                // the opposing can happen in two ways, stray midi off message,
                // where there is no on, or the note is released after being stolen from another.
                if (noteToOutSocketMap.count(note)) {
                    presently_empty.insert(noteToOutSocketMap[note]);

                    juce::MidiMessage create_message(message, samplePosition);
                    midiBufferArray[noteToOutSocketMap[note]].addEvent(create_message, samplePosition);

                    SocketToNoteMap.erase(noteToOutSocketMap[note]);
                    noteToOutSocketMap.erase(note);
                }
            }

        }
    }

    void releaseResources() override {}

    void reset() override {
        if (init_phase_done) {
            for (int i = 0; i < voices; ++i) {
                presently_empty.insert(i); // insert indexes of the sockets.
                OutputSockets[i]->setMidiBufferPointer(&midiBufferArray[i]);
                FIFO_queue.push(i);
            }

            readBuff = InputSockets[0]->getMidiBuffer();
        }
    }

    // we should actually receive change from parameter ctrl
    // even when the combobox sets the value for the first time,
    // but at that time the parent is still not known(always) to parameter ctrl,
    // so this will not get called.
    void mini_reset() {

        if (!init_phase_done) {
            int voices = InputSockets[0]->getValue()-1;

            InputSockets.clear();
            OutputSockets.clear();

            for (int i = 0; i < voices; ++i) {
                juce::String temp(i+1);
                OutputSockets.add(new GraphNode::Socket(juce::String("MIDI "+temp), direction::OUT, false));
                OutputSockets[i]->setOutputType(SocketDataType::MIDI);
            }

            InputSockets.add(new GraphNode::Socket(juce::String("MIDI IN"), direction::IN, true));
            InputSockets[0]->acceptType(SocketDataType::MIDI);

            InputSockets.add(new GraphNode::Socket(juce::String("Voice preference"), direction::IN, false));
            InputSockets[1]->addMenuParameterControl();
            InputSockets[1]->addMenuItem(juce::String("Latest"));
            InputSockets[1]->addMenuItem(juce::String("Revolver"));
            InputSockets[1]->addMenuItem(juce::String("Stack"));

            midiBufferArray.resize(voices);

            init_phase_done = true;
            this->voices = voices;

            makeAllSocketsVisible();
            resized();

        } else {

        }

    }

    ~Polyphony() {};

private:

    bool init_phase_done = false;

    juce::MidiBuffer* readBuff;

    std::vector<juce::MidiBuffer> midiBufferArray;

    int number_of_voices = -1;

    int voices = -1;
};