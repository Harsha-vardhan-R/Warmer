/*
  ==============================================================================

    Chebyshev.h
    Created: 12 May 2024 11:28:43am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include "juce_dsp/juce_dsp.h"
#include <algorithm>
#include <memory>

// This simply is a juce::Reverb wrapper.
class LadderFilterWrapper : public GraphNode {
public:

LadderFilterWrapper(int posX, int posY) : GraphNode(juce::String("Ladder Filter"), posX, posY) {

        // generate and store the window coefficients.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Signal IN"), direction::IN, true));
        InputSockets[0]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Type"), direction::IN, false));
        InputSockets[1]->addMenuParameterControl();
        InputSockets[1]->addMenuItem("LP 12");
        InputSockets[1]->addMenuItem("LP 24");
        InputSockets[1]->addMenuItem("BP 12");
        InputSockets[1]->addMenuItem("BP 24");
        InputSockets[1]->addMenuItem("HP 12");
        InputSockets[1]->addMenuItem("HP 24");

        InputSockets.add(new GraphNode::Socket(juce::String("Cutoff"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(20.0, 20000.0, 400.0f);
        
        InputSockets.add(new GraphNode::Socket(juce::String("Resonance"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 1.0, 1.2);
        
        InputSockets.add(new GraphNode::Socket(juce::String("Drive"), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->addSliderParameterControl(0.0, 1.0, 0.0);

        LadderFilterBase.setEnabled(true);
        
        makeAllSocketsVisible();

        resized();
    }

    void processGraphNode() override {

        // calculate the present sum, if this is different set the parameters of 
        // juce::Reverb again(weird trick but kind of works).
        float sumHere = InputSockets[2]->getValue()+
            InputSockets[3]->getValue()+
            InputSockets[4]->getValue();

        // This is so that we do not update the 
        // values redundantly.
        if (sum != sumHere) {

            float cutoff = std::clamp<float>(InputSockets[2]->getValue(), 20.0f, 20000.0f);
            float reso = std::clamp<float>(InputSockets[3]->getValue(), 0.0f, 1.2f);
            float drive = std::clamp<float>(3.0f*InputSockets[4]->getValue()+1.0f, 1.0f, 4.0f);

            LadderFilterBase.setCutoffFrequencyHz(cutoff);
            LadderFilterBase.setResonance(reso);
            LadderFilterBase.setDrive(drive);

            sum = sumHere;
        }

        LadderFilterBase.process(*contextPointer.get());
    }


    void releaseResources() override {};


    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        readBuff = InputSockets[0]->getBufferPointer();
        
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.numChannels = 2;
        spec.maximumBlockSize = (unsigned int)estimatedSamplesPerBlock;

        inpDataBlock.reset(new juce::dsp::AudioBlock<float>(*readBuff));
        outDataBlock.reset(new juce::dsp::AudioBlock<float>(*bufferToWritePointer));

        LadderFilterBase.prepare(spec);

        contextPointer.reset(new juce::dsp::ProcessContextNonReplacing<float>(*inpDataBlock.get(), *outDataBlock.get()));
        
        mini_reset();
    }

    // Only called when the 
    void mini_reset() override {
        
        int mode = (int)InputSockets[1]->getValue();

        if (mode == 1) {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::LPF12);
        } else if (mode == 2) {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::LPF24);
        } else if (mode == 3) {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::BPF12);
        } else if (mode == 4) {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::BPF12);
        } else if (mode == 5) {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::HPF12);
        } else {
            LadderFilterBase.setMode(juce::dsp::LadderFilterMode::HPF12);
        }


    }


private:

    std::unique_ptr<juce::dsp::AudioBlock<float>> inpDataBlock;
    std::unique_ptr<juce::dsp::AudioBlock<float>> outDataBlock;

    std::unique_ptr<juce::dsp::ProcessContextNonReplacing<float>> contextPointer;

    int prevMode = 1;

    juce::Reverb::Parameters newParameters;
    
    float sum = -1.0f;

    juce::dsp::LadderFilter<float> LadderFilterBase;

    juce::AudioBuffer<float> *readBuff = nullptr;

};


