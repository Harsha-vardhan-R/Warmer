/*
  ==============================================================================

    Distortion.h
    Created: 12 May 2024 7:18:27pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class Distort : public GraphNode {
public:

    typedef void (Distort::*callbackFunction_type)();

    Distort(int pos_x, int pos_y) : GraphNode(juce::String("Distort"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        // Type of the basic wave shape.
        InputSockets.add(new GraphNode::Socket(juce::String("Algorithm"), direction::IN, false));
        InputSockets[1]->addMenuParameterControl();
        InputSockets[1]->addMenuItem("Linear");
        InputSockets[1]->addMenuItem("Soft Clip");
        InputSockets[1]->addMenuItem("Discrete");

        // Amplitude.
        InputSockets.add(new GraphNode::Socket(juce::String("Amount"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(1.0, 5.0, 1.0f);


        makeAllSocketsVisible();

        resized();
    }

    template<int type, bool isAmountModulating>
    void generateSubProcess() {

        for (int channel = 0; channel < 2; channel++) {
            float *channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inputChannelData = readbuff->getReadPointer(channel);

            if constexpr (isAmountModulating) {
                const float* mod_buffer = modBuff->getReadPointer(channel);

                for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                    if constexpr (type == 1) {
                        channelData[i] = std::clamp( mod_buffer[i] * inputChannelData[i] , -1.0f, 1.0f);
                    } else if (type == 2) {
                        channelData[i] = 0.63661977236f * std::atan(inputChannelData[i] * mod_buffer[i]);
                    } else {
                        channelData[i] = (std::round(mod_buffer[i] * inputChannelData[i])) / std::clamp(mod_buffer[i], 1.0f, 30.0f);
                    }

                }

            } else {
                float amount = std::clamp(InputSockets[2]->getValue(), 1.0f, 4.9f);
                float am_5 = amount * 5.0f;
                float disc_ = (5.0f - amount) * 5.0f;
                float div_div = 1.0f/disc_;

                for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                    if constexpr (type == 1) {
                        channelData[i] = std::clamp( amount * inputChannelData[i] , -1.0f, 1.0f);
                    } else if (type == 2) {
                        channelData[i] = 0.63661977236f * std::atan(inputChannelData[i] * am_5);
                    } else {
                        channelData[i] = (std::round(disc_ * inputChannelData[i])) * div_div;
                    }

                }

            }
        }

    }




    void processGraphNode() override {
        callbackFunction_type callback = callBackFunction.load();
        (this->*callback)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        readbuff = InputSockets[0]->getBufferPointer();
        modBuff = InputSockets[2]->getBufferPointer();

        mini_reset();
    }

    void mini_reset() override {
        if (InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            if ((int)InputSockets[1]->getValue() == 1) {
                callBackFunction.store(&Distort::generateSubProcess<1, true>);
            } else if ((int)InputSockets[1]->getValue() == 2) {
                callBackFunction.store(&Distort::generateSubProcess<2, true>);
            } else {
                callBackFunction.store(&Distort::generateSubProcess<3, true>);
            }
        } else {
            if ((int)InputSockets[1]->getValue() == 1) {
                callBackFunction.store(&Distort::generateSubProcess<1, false>);
            } else if ((int)InputSockets[1]->getValue() == 2) {
                callBackFunction.store(&Distort::generateSubProcess<2, false>);
            } else {
                callBackFunction.store(&Distort::generateSubProcess<3, false>);
            }
        }
    }


private:

    juce::AudioBuffer<float>* readbuff = nullptr, *modBuff = nullptr;

    std::atomic<callbackFunction_type> callBackFunction;
};