/*
  ==============================================================================

    Delay.h
    Created: 12 May 2024 7:18:13pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"


class Delay : public GraphNode {
public:

    Delay(int pos_x, int pos_y) : GraphNode(juce::String("Delay"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Delay Time(ms)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(0.0f, 4000.0f, 0.0f);

        InputSockets.add(new GraphNode::Socket(juce::String("FeedbackOut"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0f, 1.0f, 0.2f);

        InputSockets.add(new GraphNode::Socket(juce::String("Mix(Dry<->Wet)"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0f, 1.0f, 0.3f);

        makeAllSocketsVisible();

        resized();
    }


    void processGraphNode() override {
        float delayTime = std::clamp(InputSockets[1]->getValue(), 0.0f, 4000.0f);
        float feedback = InputSockets[2]->getValue();
        float mix = InputSockets[3]->getValue();
        float mix_inv = 1.0-mix;
        int delaySamples = std::clamp(static_cast<int>((delayTime / 1000.0f) * sampleRate), 0, max_index);
        int write_index = (read_index + delaySamples) % size_Channel;

        for (int channel = 0; channel < 2; ++channel) {
            const float* inputSignal = readBuff->getReadPointer(channel);
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            float* delayChannelData = delayBuffer[channel].data();

            int read_index_local = read_index;
            int write_index_local = write_index;

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                delayChannelData[write_index_local] = inputSignal[i] + delayChannelData[read_index_local]*feedback;
                channelData[i] = delayChannelData[read_index_local] * mix + inputSignal[i] * mix_inv;
                read_index_local = (read_index_local + 1) % size_Channel;
                write_index_local = (write_index_local + 1) % size_Channel;
            }
        }

        read_index = (read_index + bufferToWritePointer->getNumSamples()) % size_Channel;

    }

    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        if (prev_SampleRate != sampleRate) {
            int size = sampleRate * 4; // 2 seconds max.
            size_Channel = size;
            max_index = size-1;
            delayBuffer[0].resize(size); delayBuffer[1].resize(size);
            std::fill(delayBuffer[0].begin(), delayBuffer[0].end(), 0.0f);
            std::fill(delayBuffer[1].begin(), delayBuffer[1].end(), 0.0f);
            prev_SampleRate = sampleRate;
        }

        read_index = 0;

    }

    ~Delay() {};

private:

    juce::AudioBuffer<float>* readBuff;

    std::vector<float> delayBuffer[2];

    
    int prev_SampleRate = 0;

    int max_index = 0;
    // this is the only index we need to keep track of,
    // the write  index is going to get calculated from this.
    int read_index = 0;

    int size_Channel = 0;

};
