/*
  ==============================================================================

    Noise.h
    Created: 2 May 2024 10:36:20pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

class Noise : public GraphNode {
public:

    typedef void (Noise::*callbackFunction_type)(float);

    Noise(int pos_x, int pos_y)  :  GraphNode(juce::String("Noise"), pos_x, pos_y),
                                    gen(rd()),
                                    dis(-1.0f, 1.0f) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        // Type of the basic wave shape.
        InputSockets.add(new GraphNode::Socket(juce::String("Noise Type"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("White");
        InputSockets[0]->addMenuItem("Pink");
        InputSockets[0]->addMenuItem("Brown");
        InputSockets[0]->addMenuItem("Blue");

        // Amplitude.
        InputSockets.add(new GraphNode::Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 1.0, 0.1);

        callBackFunctionNoise = &Noise::generateWhiteNoise;

        makeAllSocketsVisible();

        resized();
    }

    // functions generate for each channel.
    void generateWhiteNoise(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                // Generate two samples at a time
                channelData[i] = random.nextFloat() * 2.0f - 1.0f;
                channelData[i + 1] = random.nextFloat() * 2.0f - 1.0f;

                channelData[i] *= l;
                channelData[i + 1] *= l;
            }
        }
    }


    // pink noise relates the Amplitude to the frequency as a function nearly matching (1/(f^c)),
    // where f is the frequency and the c is some constant number between ~0 and ~3,
    // although this formula is not enough to generate the noise.
    // (copied the above 3 lines from a yt video :) )
    void generatePinkNoise(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                float white1 = random.nextFloat();
                float white2 = random.nextFloat();

                // Generate two samples at a time
                channelData[i] = ( white1 * pinkParamTwo) + (prevSample1 * pinkParamOne);
                channelData[i + 1] = ( white2 * pinkParamTwo) + (prevSample2 * pinkParamOne);

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= l;
                channelData[i + 1] *= l;

            }
        }
    }

    void generateBrownNoise(float l) noexcept {
        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                float white = random.nextFloat();
                channelData[i] = prevSample + (0.02f * white);
                prevSample = channelData[i];
                channelData[i] *= l;
            }
        }
    }

    void generateBlueNoise(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                float white1 = random.nextFloat();
                float white2 = random.nextFloat();

                // Generate two samples at a time
                channelData[i] = (0.01f * prevSample1) + white1;
                channelData[i + 1] = (0.01f * prevSample2) + white2;

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= l;
                channelData[i + 1] *= l;

            }
        }
    }

    // functions generate for each channel.
    void generateWhiteNoise_mod(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* loudness = loudness_mod->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                // Generate two samples at a time
                channelData[i] = random.nextFloat() * 2.0f - 1.0f;
                channelData[i + 1] = random.nextFloat() * 2.0f - 1.0f;

                channelData[i] *= loudness[i];
                channelData[i + 1] *= loudness[i+1];
            }
        }
    }

    void generatePinkNoise_mod(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* loudness = loudness_mod->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                float white1 = random.nextFloat();
                float white2 = random.nextFloat();

                // Generate two samples at a time
                channelData[i] = ( white1 * pinkParamTwo) + (prevSample1 * pinkParamOne);
                channelData[i + 1] = ( white2 * pinkParamTwo) + (prevSample2 * pinkParamOne);

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= loudness[i];
                channelData[i + 1] *= loudness[i+1];

            }
        }
    }

    void generateBrownNoise_mod(float l) noexcept {
        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* loudness = loudness_mod->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                float white = random.nextFloat();
                channelData[i] = prevSample + (0.02f * white);
                prevSample = channelData[i];
                channelData[i] *= loudness[i];
            }
        }
    }

    void generateBlueNoise_mod(float l) noexcept {

        for (int channel = 0; channel < 2; ++channel) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* loudness = loudness_mod->getReadPointer(channel);

            for (int i = 0; i < bufferToWritePointer->getNumSamples(); i += 2) {
                float white1 = random.nextFloat();
                float white2 = random.nextFloat();

                // Generate two samples at a time
                channelData[i] = (0.01f * prevSample1) + white1;
                channelData[i + 1] = (0.01f * prevSample2) + white2;

                prevSample1 = channelData[i];
                prevSample2 = channelData[i + 1];

                channelData[i] *= loudness[i];
                channelData[i + 1] *= loudness[i+1];

            }
        }
    }


    void mini_reset() override {
        int ID = (int)std::round(InputSockets[0]->getValue());
        bool is_modulated = InputSockets[1]->isThisConnected();

        callbackFunction_type callBackTempPointer;

        if (!is_modulated) {
            if (ID == 1) callBackTempPointer = &Noise::generateWhiteNoise;
            else if (ID == 2) callBackTempPointer = &Noise::generatePinkNoise;
            else if (ID == 3) callBackTempPointer = &Noise::generateBrownNoise;
            else callBackTempPointer = &Noise::generateBlueNoise;
        } else {
            if (ID == 1) callBackTempPointer = &Noise::generateWhiteNoise_mod;
            else if (ID == 2) callBackTempPointer = &Noise::generatePinkNoise_mod;
            else if (ID == 3) callBackTempPointer = &Noise::generateBrownNoise_mod;
            else callBackTempPointer = &Noise::generateBlueNoise_mod;
        }

        callBackFunctionNoise.store(callBackTempPointer);
    }

    void processGraphNode() override {
        float gain = InputSockets[1]->getValue();
        
        // calling the function pointer that is set to the
        // correct function (hopefully :( ).
        callbackFunction_type callback = callBackFunctionNoise.load();
        (this->*callBackFunctionNoise)(gain);
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        pinkParamOne = std::exp((-2.0 * PI * 30.0) / sampleRate);
        pinkParamTwo = std::sqrt( 1.0 - (pinkParamOne * pinkParamOne) );

        prevSample1 = 0.0f;
        prevSample2 = 0.0f;

        prevSample = 0.0f;

        loudness_mod = InputSockets[1]->getBufferPointer();

        int ID = (int)std::round(InputSockets[0]->getValue());
        bool is_modulated = InputSockets[1]->isThisConnected();

        callbackFunction_type callBackTempPointer;

        if (!is_modulated) {
            if (ID == 1) callBackTempPointer = &Noise::generateWhiteNoise;
            else if (ID == 2) callBackTempPointer = &Noise::generatePinkNoise;
            else if (ID == 3) callBackTempPointer = &Noise::generateBrownNoise;
            else callBackTempPointer = &Noise::generateBlueNoise;
        } else {
            if (ID == 1) callBackTempPointer = &Noise::generateWhiteNoise_mod;
            else if (ID == 2) callBackTempPointer = &Noise::generatePinkNoise_mod;
            else if (ID == 3) callBackTempPointer = &Noise::generateBrownNoise_mod;
            else callBackTempPointer = &Noise::generateBlueNoise_mod;
        }

        callBackFunctionNoise.store(callBackTempPointer);

    }

    ~Noise() {};

private:

    // random device and the generator initializations.
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    juce::Random random;

    float pinkParamOne = 0.0;
    float pinkParamTwo = 0.0;

    float prevSample1 = 0.0f;
    float prevSample2 = 0.0f;

    float prevSample = 0.0f;

    juce::AudioBuffer<float>* loudness_mod = nullptr;

    std::atomic<callbackFunction_type> callBackFunctionNoise;
};