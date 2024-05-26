/*
  ==============================================================================

    ADSR.h
    Created: 12 May 2024 11:27:00am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

class ADSR : public GraphNode {
public:

    ADSR(int pos_x, int pos_y) : GraphNode(juce::String("ADSR"), pos_x, pos_y) {

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("MIDI"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("Attack(ms)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(0.0, 6000.0, 20.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Delay(ms)"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0, 6000.0, 20.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Sustain"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.4);

        InputSockets.add(new GraphNode::Socket(juce::String("Release(ms)"), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(0.0, 6000.0, 60.0);

        makeAllSocketsVisible();

        resized();
    }

    enum class section {
        off,
        AD_section, // sustain is also in this part.
        release_section
    };

    // SUS
    // sample should NOT be 0
    float ad_fill(int a, int d, float sus, int sample) {
        if (sample < a) { // Attack phase
            return static_cast<float>(sample) / static_cast<float>(a);
        } else if (sample < a + d) { // Decay phase
            return 1.0f + (sus - 1.0f) * (static_cast<float>(sample - a) / static_cast<float>(d));
        } else { // Sustain phase
            return sus;
        }
    }

    float release_fill(int r, float sus, int sample) {
        return sus * (1.0f - (static_cast<float>(sample) / static_cast<float>(r)));
    }

    void processGraphNode() override {

        float* channelData = bufferToWritePointer->getWritePointer(0);
        int channel_size = bufferToWritePointer->getNumSamples();

        int i = 0;
        juce::MidiMessage message;


        for (const auto metadata : *readBuff) {
            message = metadata.getMessage();
            samplePosition = metadata.samplePosition; // will always be inside the bounds.

            int note = message.getNoteNumber();

            // any note press will steal the voice without back tracing, so use a proper number of voices.
            if (message.isNoteOn()) {
                // start creating envelope for this event.
                pressed_note = note;
                index = 1;
                current_section = section::AD_section;
                // get the ADSR values in terms of number of samples
                // these values will not change until the envelope for this
                // note ends and a new one starts.
                A_ = std::clamp(static_cast<int>((InputSockets[1]->getValue() / 1000.0f) * sampleRate), 1, INT_MAX);
                D_ = std::clamp(static_cast<int>((InputSockets[2]->getValue() / 1000.0f) * sampleRate), 1, INT_MAX);
                S_ = std::clamp(InputSockets[3]->getValue(), 0.0f, 1.0f);
                R_ = std::clamp(static_cast<int>((InputSockets[4]->getValue() / 1000.0f) * sampleRate), 1, INT_MAX);
            } else if (message.isNoteOff() && pressed_note == note) { // we do not do anything for stray off messages.
                index = 0; // this will be needed by the release_fill.
                current_section = section::release_section;
            }

            // `i` keeps track of the current index in the block that we are processing, while `index` keeps track of the position
            // we are at in the ADSR envelope.
            if (current_section == section::off) // fill the buffer till now from the last event as 0.
                for (; i < samplePosition; ++index, ++i) channelData[i] = 0.0;
            else if (current_section == section::AD_section)
                for (; i < samplePosition; ++index, ++i) channelData[i] = ad_fill( A_, D_, S_, index);
            else {
                for (; i < samplePosition; ++index, ++i) {
                    channelData[i] = release_fill( R_, S_, index);
                    if (index > R_) {
                        current_section = section::off;
                        break;
                    }
                }
            }

        }

        for (; i < channel_size; ++i, ++index) {
            if (current_section == section::off) {
                channelData[i] = 0.0f;
            } else if (current_section == section::AD_section) {
                channelData[i] = ad_fill(A_, D_, S_, index);
            } else if (current_section == section::release_section) {
                channelData[i] = release_fill(R_, S_, index);
                if (index >= R_) {
                    current_section = section::off;
                }
            }
        }

        // copy the entire thing to right channel;
        float* channelData_right = bufferToWritePointer->getWritePointer(1);
        for (int j = 0; j < channel_size; ++j) channelData_right[j] = channelData[j];
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        readBuff = InputSockets[0]->getMidiBuffer();

        pressed_note = -1;

        A_ = 0;
        D_ = 0;
        S_ = 0.0;
        R_ = 0;

        current_section = section::off;

    }

    void mini_reset() {}

    ~ADSR() {};

private:

    int pressed_note = -1;

    juce::MidiBuffer* readBuff;

    int stage = -1;

    int A_ = 0;
    int D_ = 0;
    float S_ = 0.0;
    int R_ = 0;

    int samplePosition = 0;

    section current_section = section::off;

    int index = 0; // keep track of at which part of the envelope we are in.

};