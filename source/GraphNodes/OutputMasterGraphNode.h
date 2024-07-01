/*
  ==============================================================================

    OutputMasterGraphNode.h
    Created: 4 Mar 2024 9:59:22pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNode.h"
#include "juce_audio_basics/juce_audio_basics.h"

class OutputMasterGraphNode : public GraphNode {
public:

    OutputMasterGraphNode(int pos_x, int pos_y) :
            GraphNode("AUDIO OUT",
                      pos_x,
                      pos_y) {

        needAudioBuffer = false;

        InputSockets.add(new Socket(juce::String("Audio Out"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        canBeDeleted = false;

        setMenuResultID(6666);

        makeAllSocketsVisible();

        resized();
    };

    void processGraphNodeOutput(juce::AudioBuffer<float> outputChannelData, float volumeLevel) {


        for (int channel = 0; channel < 2; channel++) {
            const float* finalProcessedSignal = readBuff->getReadPointer(channel);
            float* outputToHearChannelData = outputChannelData.getWritePointer(channel);
            float* outputToDisplayChannelData = displayBuff->getWritePointer(channel);

            const juce::ScopedLock sl(lock);

            for (int i = 0; i < readBuff->getNumSamples();  i += 2) {
                outputToDisplayChannelData[i] = finalProcessedSignal[i];
                outputToDisplayChannelData[i+1] = finalProcessedSignal[i+1];

                outputToHearChannelData[i] = finalProcessedSignal[i]*volumeLevel;
                outputToHearChannelData[i+1] = finalProcessedSignal[i+1]*volumeLevel;
            }
        }


    }

    ~OutputMasterGraphNode() override {
        if (displayBuff) delete displayBuff;
    };

    bool allGood() override { return (InputSockets[0]->isThisConnected()); }

    const juce::AudioBuffer<float>& getBufferToDisplay() {
        return *displayBuff;
    }

    void processGraphNode() override {}

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();

        if (displayBuff && displayBuff->getNumSamples() == estimatedSamplesPerBlock) return;

        if (displayBuff) delete displayBuff;
        displayBuff = new juce::AudioBuffer<float>(2, estimatedSamplesPerBlock);
    }

private:
    juce::CriticalSection lock;

    juce::AudioBuffer<float>* readBuff = nullptr;

    juce::AudioBuffer<float>* displayBuff = nullptr;

};
