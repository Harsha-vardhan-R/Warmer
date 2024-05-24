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
        OutputSockets[1]->setOutputType(SocketDataType::AudioBufferFloat);

        OutputSockets.add(new Socket(juce::String("Mod Wheel"), direction::OUT, false));
        OutputSockets[2]->setOutputType(SocketDataType::AudioBufferFloat);

        canBeDeleted = false;
        makeAllSocketsVisible();
        resized();
    };

    // callback functions.
//    template < bool processMidi, bool processModWheel, bool processPitchWheel>
//    void subProcessCallback() {
//
//        if constexpr (processMidi) {
//            // a little clean such that no note numbers except [0..=127] are entering.
//            juce::MidiMessage message;
//
//            // Iterate through all the messages in the buffer
//            for (const auto metadata : *midiBuffer) {
//                message = metadata.getMessage();
//
//                // fill the `freqAtSamplePoint_from_MIDI` till here.
//                // no need to check for less than number of samples because `samplePosition` is always less than the number of samples.
//                for (; currentIndexPosition < samplePosition; ++currentIndexPosition) {
//                    freqAtSamplePoint_from_MIDI[currentIndexPosition] = presMIDI_freq;
//                }
//
//                int note = std::clamp(message.getNoteNumber(), 0, 127); // makes sure we do not access outside the `midiFrequencies[128]`
//
//                // Check if the message is a Note On or Note Off message
//                if (message.isNoteOn()) {
//                    MIDI_order.insert(note);
//                    presMIDI_Note = note;
//                    presMIDI_freq = midiFrequencies[note];
//                } else if (message.isNoteOff()) {
//                    MIDI_order.erase(note);
//                    int note_now = getLastElement(MIDI_order);
//                    // we reset if there is new note is different.
//                    if (note_now != presMIDI_Note) resetPhaseAt.insert(samplePosition);
//                    if (note_now == -1) {
//                        presMIDI_Note = -1;
//                        presMIDI_freq = 0;
//                    } else {
//                        presMIDI_Note = note_now; // Fixing this to use note_now instead of note
//                        presMIDI_freq = midiFrequencies[note_now];
//                    }
//                }
//            }
//        } else { // no need to worry about setting the buffer.
//
//        }
//    }

    void processGraphNode() override {

    }

    void setInputMIDI(juce::MidiBuffer* buffer) {
        readBuff = buffer;
    }

    void releaseResources() override {}
    void reset() override {
        OutputSockets[0]->setMidiBufferPointer(readBuff);
    }

    ~InputMasterGraphNode() {};

private:

    juce::MidiBuffer* readBuff;

    void (InputMasterGraphNode::*callBackFunction)() = nullptr;

};