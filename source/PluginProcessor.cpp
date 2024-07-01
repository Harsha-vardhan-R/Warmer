#include "PluginProcessor.h"
#include "MainComponent.h"
#include "PluginEditor.h"
#include "Instrument.h"


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
//#ifdef JUCE_DEBUG
//    std::cout << "Global Function createPluginFilter Called" << "\n";
//#endif
    return new PluginProcessor();
}


//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)) {
   
         // The Main MainComponent will be created before this scope
         // in the constructor.
         InstrumentInstancePointer = Instrument::getInstance();

}

PluginProcessor::~PluginProcessor() {
}

//==============================================================================
const juce::String PluginProcessor::getName() const {
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const {
    return true;
}

bool PluginProcessor::producesMidi() const {
    return false;
}

bool PluginProcessor::isMidiEffect() const {
    return false;
}

double PluginProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PluginProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram() {
    return 0;
}

void PluginProcessor::setCurrentProgram (int index) {
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index) {
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName) {
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    if (InstrumentInstancePointer) InstrumentInstancePointer->prepareNodeProcessingQueue(sampleRate, samplesPerBlock);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    return true;
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (InstrumentInstancePointer) InstrumentInstancePointer->midiInAndProcessAudioInplace(midiMessages, buffer);
}

//==============================================================================
bool PluginProcessor::hasEditor() const {
    return true; 
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
    return new PluginEditor (&MainComponentInstance, *this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

