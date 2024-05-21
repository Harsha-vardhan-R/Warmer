/*
  ==============================================================================

    Utility.h
    Created: 12 May 2024 11:25:55am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Utility : public GraphNode {
public:

    Utility(int pos_x, int pos_y) : GraphNode(juce::String("Utility"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new Socket(juce::String("Linear Gain"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(-1.0, 5.0, 1.0);

        InputSockets.add(new Socket(juce::String("Pan Amount"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(-1.0, 1.0, 0.0);

        makeAllSocketsVisible();

        resized();
    }

    void noModulation(float gain, float pan) {

        float leftGain = ((pan <= 0.0f) ? 1.0f : 1.0f - pan) * gain;
        float rightGain = ((pan >= 0.0f) ? 1.0f : 1.0f + pan) * gain;

        float gains[2] = { leftGain, rightGain };

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);

            float gainHere = gains[channel];

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = gainHere * inputSignal[i];
                channelData[i+1] = gainHere * inputSignal[i+1];
            }
        }

    }

    void onlyGainModulating(float gain, float pan) {

        float right_tot_gain = (pan <= 0.0f) ? 1.0f : 1.0f - pan;
        float left_tot_gain = (pan >= 0.0f) ? 1.0f : 1.0f + pan;
        float tot_gain[2] = { right_tot_gain, left_tot_gain };

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputSignal = readBuff->getReadPointer(channel);
            float gain_here = tot_gain[channel];
            const float* gain_mod = ModulatingBuffer_gain->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                channelData[i] = gain_here*inputSignal[i] * gain_mod[i];
                channelData[i+1] = gain_here*inputSignal[i+1] * gain_mod[i+1];
            }
        }

    }

    // because we treat the left and right buffers individually even for this,
    // we can get really cool effects if the input is already stereo wide.
    // but may cause a problem if the input pan modulation is way too stereo wide,
    // like nearly out of phase. then you may not actually get the desired effect.
    void onlyPanModulating(float gain, float pan) {

        float* channelData = bufferToWritePointer->getWritePointer(0);
        const float* inputSignal = readBuff->getReadPointer(0);
        const float* pan_mod = ModulatingBuffer_pan->getReadPointer(0);


        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
            float pan_gain = (pan_mod[i] <= 0.0f) ? 1.0f : 1.0f - pan_mod[i];
            channelData[i] = pan_gain*inputSignal[i] * gain;
        }

        float* channelData_rt = bufferToWritePointer->getWritePointer(1);
        const float* inputSignal_rt = readBuff->getReadPointer(1);
        const float* pan_mod_rt = ModulatingBuffer_pan->getReadPointer(1);


        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
            float pan_gain = (pan_mod_rt[i] >= 0.0f) ? 1.0f : 1.0f + pan_mod_rt[i];
            channelData_rt[i] = pan_gain*inputSignal_rt[i] * gain;
        }

    }

    void bothModulating(float gain, float pan) {

        float* channelData = bufferToWritePointer->getWritePointer(0);
        const float* inputSignal = readBuff->getReadPointer(0);
        const float* pan_mod = ModulatingBuffer_pan->getReadPointer(0);
        const float* gain_mod = ModulatingBuffer_gain->getReadPointer(0);

        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
            float pan_gain = (pan_mod[i] <= 0.0f) ? 1.0f : 1.0f - pan_mod[i];
            channelData[i] = pan_gain*inputSignal[i] * gain_mod[i];
        }

        float* channelData_rt = bufferToWritePointer->getWritePointer(1);
        const float* inputSignal_rt = readBuff->getReadPointer(1);
        const float* pan_mod_rt = ModulatingBuffer_pan->getReadPointer(1);
        const float* gain_mod_rt = ModulatingBuffer_gain->getReadPointer(1);

        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
            float pan_gain = (pan_mod_rt[i] >= 0.0f) ? 1.0f : 1.0f + pan_mod_rt[i];
            channelData_rt[i] = pan_gain*inputSignal_rt[i] * gain_mod_rt[i];
        }

    }

    void processGraphNode() override {
        float gain = InputSockets[1]->getValue();
        float pan = InputSockets[2]->getValue();

        (this->*callbackFunction)(gain, pan);
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        ModulatingBuffer_gain = InputSockets[1]->getBufferPointer();
        ModulatingBuffer_pan = InputSockets[2]->getBufferPointer();

        readBuff = InputSockets[0]->getBufferPointer();

        if (InputSockets[1]->isThisConnected() && InputSockets[2]->isThisConnected()) {
            callbackFunction = &Utility::bothModulating;
        } else if (InputSockets[1]->isThisConnected()) {
            callbackFunction = &Utility::onlyGainModulating;
        } else if (InputSockets[2]->isThisConnected()) {
            callbackFunction = &Utility::onlyPanModulating;
        } else {
            callbackFunction = &Utility::noModulation;
        }
    }

    ~Utility() {};

private:

    // the signal IN.
    juce::AudioBuffer<float>* readBuff;

    juce::AudioBuffer<float>* ModulatingBuffer_gain = nullptr;
    juce::AudioBuffer<float>* ModulatingBuffer_pan = nullptr;


    void (Utility::*callbackFunction)(float, float) = nullptr;

};
