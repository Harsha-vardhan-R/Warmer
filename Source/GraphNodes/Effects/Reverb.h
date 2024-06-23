/*
  ==============================================================================

    Reverb.h
    Created: 12 May 2024 7:16:55pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <algorithm>

#define SQRT_TWO 1.41421356237

// This simply is a juce::Reverb wrapper.
class Reverb : public GraphNode {
public:

Reverb(int posX, int posY) : GraphNode(juce::String("Reverb"), posX, posY) {

        // generate and store the window coefficients.

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Size"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->addSliderParameterControl(0.0, 1.0, 0.3);

        InputSockets.add(new GraphNode::Socket(juce::String("Damp"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0, 1.0, 0.5);

        InputSockets.add(new GraphNode::Socket(juce::String("Width"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 0.8);

        InputSockets.add(new GraphNode::Socket(juce::String("Mix"), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(0.0, 1.0, 0.2);
        
        makeAllSocketsVisible();

        resized();
    }

    void processGraphNode() override {

        // calculate the present sum, if this is different set the parameters of 
        // juce::Reverb again(weird trick but kind of works).
        float sumHere = InputSockets[1]->getValue()+
            InputSockets[2]->getValue()+
            InputSockets[3]->getValue()+
            InputSockets[4]->getValue();

        // This is so that we do not call the 
        if (sum != sumHere) {
            newParameters.roomSize = InputSockets[1]->getValue();
            newParameters.damping = InputSockets[2]->getValue();
            newParameters.width = InputSockets[3]->getValue();
            
            float mix = std::clamp<float>(InputSockets[4]->getValue(), 0.0, 1.0);
            newParameters.dryLevel = 1.0f-mix;
            newParameters.wetLevel = mix;

            // Set the new changed parameters.
            juceReverbBase.setParameters(newParameters);


            sum = sumHere;
        }

        // because this is a inplace algo, first copy the data to output buffer.
        //
        int numSamples = bufferToWritePointer->getNumSamples();

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* inpData = readBuff->getReadPointer(channel);

            for (int i = 0; i < numSamples; ++i) channelData[i] = inpData[i];
        }

        float* leftChInp = bufferToWritePointer->getWritePointer(0);
        float* rightChInp = bufferToWritePointer->getWritePointer(1);

        // Give the buffer to the juce built-in Reverb.
        juceReverbBase.processStereo(leftChInp, rightChInp, numSamples);
    }


    void releaseResources() override {};


    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        readBuff = InputSockets[0]->getBufferPointer();
        
        juceReverbBase.setSampleRate(sampleRate);
    } 


private:

    juce::Reverb::Parameters newParameters;
    
    float sum = -1.0f;

    juce::Reverb juceReverbBase;

    juce::AudioBuffer<float> *readBuff = nullptr;

};


