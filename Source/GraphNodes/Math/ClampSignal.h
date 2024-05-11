/*
  ==============================================================================

    ClampSignal.h
    Created: 8 May 2024 9:35:05pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Clamp : public GraphNode {
public:

    Clamp(int pos_x, int pos_y) : GraphNode(juce::String("Clamp Signal"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);


        InputSockets.add(new Socket(juce::String("Upper Bound"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 1.0, 1.0);

        InputSockets.add(new Socket(juce::String("Lower Bound"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(0.0, 1.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModProcesses() {
        float upperBound = InputSockets[1]->getValue();
        float lowerBound = InputSockets[2]->getValue();

        for (int channel = 0; channel < 2; channel++) {
            const float *channelData = readBuff->getReadPointer(channel);
            float *writeChannel = bufferToWritePointer->getWritePointer(channel);
            for (int i = 0; i < readBuff->getNumSamples(); ++i) {
                float data = channelData[i];
                float absolute = std::abs(data);
                float sign = data / absolute;
                writeChannel[i] = std::clamp(absolute, lowerBound, upperBound) * sign;
            }
        }
    }

    void onlyLowerModulating() {
        float upperBound = InputSockets[1]->getValue();

        for (int channel = 0; channel < 2; channel++) {
            const float* channelData = readBuff->getReadPointer(channel);
            float* writeChannel = bufferToWritePointer->getWritePointer(channel);

            const float* lowerBoundModulating = lowerBoundModulatingBuffer->getReadPointer(channel);

            for (int i = 0 ; i < readBuff->getNumSamples(); ++i) {
                float data = channelData[i];
                float absolute = std::abs(data);
                float sign = data/absolute;
                writeChannel[i] = std::clamp(absolute, lowerBoundModulating[i], upperBound) * sign;
            }
        }

    }

    void onlyUpperModulating() {
        float lowerBound = InputSockets[2]->getValue();

        for (int channel = 0; channel < 2; channel++) {
            const float* channelData = readBuff->getReadPointer(channel);
            float* writeChannel = bufferToWritePointer->getWritePointer(channel);

            const float* upperBoundModulating = upperBoundModulatingBuffer->getReadPointer(channel);

            for (int i = 0 ; i < readBuff->getNumSamples(); ++i) {
                float data = channelData[i];
                float absolute = std::abs(data);
                float sign = data/absolute;
                writeChannel[i] = std::clamp(absolute, lowerBound, upperBoundModulating[i]) * sign;
            }
        }
    }

    void bothBoundsModulating() {
        for (int channel = 0; channel < 2; channel++) {
            const float* channelData = readBuff->getReadPointer(channel);
            float* writeChannel = bufferToWritePointer->getWritePointer(channel);

            const float* lowerBoundModulating = lowerBoundModulatingBuffer->getReadPointer(channel);
            const float* upperBoundModulating = upperBoundModulatingBuffer->getReadPointer(channel);

            for (int i = 0 ; i < readBuff->getNumSamples(); ++i) {
                float data = channelData[i];
                float absolute = std::abs(data);
                float sign = data/absolute;
                writeChannel[i] = std::clamp(absolute, lowerBoundModulating[i], upperBoundModulating[i]) * sign;
            }
        }
    }

    void processGraphNode() override {
        (this->*callbackFunction)();
    }


    void releaseResources() override {}

    void reset() override {
        readBuff = InputSockets[0]->getBufferPointer();
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        bool upperModulated = InputSockets[1]->isThisConnected();
        bool lowerModulated = InputSockets[2]->isThisConnected();

        // if not being modulated they will be set to nullptr, not a problem.
        upperBoundModulatingBuffer = InputSockets[1]->getBufferPointer();
        lowerBoundModulatingBuffer = InputSockets[2]->getBufferPointer();

        // set the callback function.
        if (upperModulated && lowerModulated) {
            callbackFunction = &Clamp::bothBoundsModulating;
        } else if (upperModulated) {
            callbackFunction = &Clamp::onlyUpperModulating;
        } else if (lowerModulated) {
            callbackFunction = &Clamp::onlyLowerModulating;
        } else {
            callbackFunction = &Clamp::noModProcesses;
        }

    }

    ~Clamp() {};

private:

    double currentAngle;
    double angleDelta;

    juce::MidiBuffer midiBuffer;

    juce::AudioBuffer<float>* readBuff;

    juce::AudioBuffer<float>* upperBoundModulatingBuffer = nullptr;
    juce::AudioBuffer<float>* lowerBoundModulatingBuffer = nullptr;

    void (Clamp::*callbackFunction)() = nullptr;

};