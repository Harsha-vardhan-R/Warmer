#include <JuceHeader.h>
#include "WaveShapeDisp.h"

WaveShapeDisp* WaveShapeDisp::this_instance = nullptr;

//==============================================================================
WaveShapeDisp::WaveShapeDisp(int channel) : juce::AudioVisualiserComponent(channel) {
    WaveShapeDisp::this_instance = this;

    setColours(juce::Colours::white, juce::Colours::grey);
    setRepaintRate(40);
    setSamplesPerBlock(1);
    //addAndMakeVisible(&audioVisualiser);
    setBufferSize(512);

    setOpaque(true);
}

void WaveShapeDisp::changeBufferSize(int bufferSize) {
    //setBufferSize(bufferSize);
}

WaveShapeDisp::~WaveShapeDisp() {}

void WaveShapeDisp::resized() {
    setBounds(0 , 0 , getWidth() , getHeight());
}

void WaveShapeDisp::start() {
    borderColour = juce::Colours::green;
}

void WaveShapeDisp::stop() {
    borderColour = juce::Colours::grey;
}

void WaveShapeDisp::trigger() {}

void WaveShapeDisp::pushFreshBuffer(const juce::AudioBuffer<float>& buffer) {
    const juce::ScopedLock sl(lock);
    pushBuffer(buffer);
}
