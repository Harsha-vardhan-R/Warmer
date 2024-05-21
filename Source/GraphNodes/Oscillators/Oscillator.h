/*
  ==============================================================================

    Oscillator.h
    Created: 27 Mar 2024 4:16:44pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"


class Oscillator : public GraphNode {
public:

    float midiFrequencies[128] = {
            // midi frequencies by octave, first row is below C0, octaves here start with c.                                                              A
            8.1757989f,    8.6619572f,     9.1770240f,   9.7227182f,    10.3008612f,    10.9133822f,    11.5623257f,    12.2498574f,   12.9782718f,   13.7500000f,   14.5676175f,   15.4338532f,
            16.3515978f,   17.3239144f,   18.3540480f,   19.4454365f,   20.6017223f,    21.8267645f,    23.1246514f,    24.4997147f,   25.9565436f,   27.5000000f,   29.1352351f,   30.8677063f, // 0
            32.7031957f,   34.6478289f,   36.7080960f,   38.8908730f,   41.2034446f,    43.6535289f,    46.2493028f,    48.9994295f,   51.9130872f,   55.0000000f,   58.2704702f,   61.7354127f,
            65.4063913f,   69.2956577f,   73.4161920f,   77.7817459f,   82.4068892f,    87.3070579f,    92.4986057f,    97.9988590f,   103.8261744f,  110.0000000f,  116.5409404f,  123.4708253f,
            130.8127827f,  138.5913155f,  146.8323840f,  155.5634919f,  164.8137785f,   174.6141157f,   184.9972114f,   195.9977180f,  207.6523488f,  220.0000000f,  233.0818808f,  246.9416506f,
            261.6255653f,  277.1826310f,  293.6647679f,  311.1269837f,  329.6275569f,   349.2282314f,   369.9944227f,   391.9954360f,  415.3046976f,  440.0000000f,  466.1637615f,  493.8833013f,
            523.2511306f,  554.3652620f,  587.3295358f,  622.2539674f,  659.2551138f,   698.4564629f,   739.9888454f,   783.9908720f,  830.6093952f,  880.0000000f,  932.3275230f,  987.7666025f,
            1046.5022612f, 1108.7305239f, 1174.6590717f, 1244.5079349f, 1318.5102277f,  1396.9129257f,  1479.9776908f,  1567.9817439f, 1661.2187903f, 1760.0000000f, 1864.6550461f, 1975.5332050f,
            2093.0045224f, 2217.4610478f, 2349.3181433f, 2489.0158698f, 2637.0204553f,  2793.8258515f,  2959.9553817f,  3135.9634879f, 3322.4375806f, 3520.0000000f, 3729.3100921f, 3951.0664100f,
            4186.0090448f, 4434.9220956f, 4698.6362867f, 4978.0317396f, 5274.0409106f,  5587.6517029f,  5919.9107634f,  6271.9269757f, 6644.8751613f, 7040.0000000f, 7458.6201843f, 7902.1328201f, // 8
            8372.0180896f, 8869.8441913f, 9397.2725734f, 9956.0634791f, 10548.0818212f, 11175.3034059f, 11839.8215268f, 12543.8539514f
    };

    Oscillator(int pos_x, int pos_y) : GraphNode(juce::String("Oscillator"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        // Type of the basic wave shape.
        InputSockets.add(new Socket(juce::String("Wave Shape"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("Sine");
        InputSockets[0]->addMenuItem("Square");
        InputSockets[0]->addMenuItem("Triangle");
        InputSockets[0]->addMenuItem("Saw-tooth");

        // frequency.
        InputSockets.add(new Socket(juce::String("Frequency (Hz)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::MIDI);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 20000.0, 150.0); // frequency range.

        // Amplitude.
        InputSockets.add(new Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[2]->addSliderParameterControl(0.0, 1.0, 0.5);

        // Phase.
        InputSockets.add(new Socket(juce::String("Phase (deg)"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[3]->addSliderParameterControl(0.0, 2*PI, 0.0);

        makeAllSocketsVisible();

        resized();
    }


    enum class freqInputType {
        Floating,
        AudioBuffer,
        MIDI
    };

    enum class waveType {
        sine,
        sawtooth,
        triangle,
        square
    };

    enum class boolType {
        Floating,
        AudioBuffer
    };

    // returns the last element from a set.
    // if the set is empty returns `-1`
    int getLastElement(const std::set<int>& inputSet) {
        if (inputSet.empty()) return -1;
        else return *inputSet.rbegin();
    }

    //  wave-shape, freqSocketIn  isAmplitudeConnected, isPhaseConnected.
    template <waveType waveInputType, freqInputType freqType, boolType amplType, boolType phaseType>
    void generateProcessCallback(float freq, float amplitude, float phase) {
        // in this method we calculate the change in the phase of the phase angle.
        // and calculate the samples based on the change in phase.

        for (int channel = 0; channel < 2; channel++) {
            float *channelData = bufferToWritePointer->getWritePointer(channel);
            float current_phase = phase_set[channel];

            if constexpr(freqType == freqInputType::AudioBuffer) {
                const float *freqModBuffer = frequencyModBuffer->getReadPointer(channel);

                if constexpr(phaseType == boolType::Floating) { // phase is just now an offset
                    current_phase = current_phase + phase;

                    if constexpr(amplType == boolType::Floating) {
                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqModBuffer[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }
                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqModBuffer[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }
                    }

                    current_phase = current_phase - phase; // if this is not done the phase offset keeps on increasing for every process call.

                } else { // here phase is a changing offset.
                    const float *phaseModPointer = phaseModBuffer->getReadPointer(channel);

                    if constexpr(amplType == boolType::Floating) {
                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += (freqModBuffer[i] * phaseIncrementRatio) + phaseModPointer[i];
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }
                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqModBuffer[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }
                    }
                }

            } else if (freqType == freqInputType::MIDI) {
                std::unordered_set<int> resetPhaseAt;

                // we create the freqAtSamplePoint_from_MIDI from the midi buffer.
                juce::MidiMessage message;
                int samplePosition;

                int currentIndexPosition = 0;

                // Iterate through all the messages in the buffer
                for (const auto metadata : *midiBuffer) {
                    message = metadata.getMessage();
                    samplePosition = metadata.samplePosition;

                    // fill the `freqAtSamplePoint_from_MIDI` till here.
                    // no need to check for less than number of samples because `samplePosition` is always less than the number of samples.
                    for (; currentIndexPosition < samplePosition; ++currentIndexPosition) {
                        freqAtSamplePoint_from_MIDI[currentIndexPosition] = presMIDI_freq;
                    }

                    int note = std::clamp(message.getNoteNumber(), 0, 127); // makes sure we do not access outside the `midiFrequencies[128]`

                    // Check if the message is a Note On or Note Off message
                    if (message.isNoteOn()) {
                        MIDI_order.insert(note);
                        presMIDI_Note = note;
                        presMIDI_freq = midiFrequencies[note];
                    } else if (message.isNoteOff()) {
                        MIDI_order.erase(note);
                        int note_now = getLastElement(MIDI_order);
                        // we reset if there is new note is different.
                        if (note_now != presMIDI_Note) resetPhaseAt.insert(samplePosition);
                        if (note_now == -1) {
                            presMIDI_Note = -1;
                            presMIDI_freq = 0;
                        } else {
                            presMIDI_Note = note_now; // Fixing this to use note_now instead of note
                            presMIDI_freq = midiFrequencies[note_now];
                        }
                    }
                }

                // Fill remaining samples in `freqAtSamplePoint_from_MIDI`
                for (; currentIndexPosition < bufferToWritePointer->getNumSamples(); ++currentIndexPosition) {
                    freqAtSamplePoint_from_MIDI[currentIndexPosition] = presMIDI_freq;
                }

                if constexpr(phaseType == boolType::Floating) { // phase is just now an offset
                    current_phase = current_phase + phase;

                    if constexpr(amplType == boolType::Floating) {
                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);
                            if (resetPhaseAt.count(i)) current_phase = 0;

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }
                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);
                            if (resetPhaseAt.count(i)) current_phase = 0;

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }
                    }
                    current_phase = current_phase - phase; // if this is not done the phase offset keeps on increasing for every process call.
                } else { // here phase is a changing offset.
                    const float *phaseModPointer = phaseModBuffer->getReadPointer(channel);

                    if constexpr(amplType == boolType::Floating) {
                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += (freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio) + phaseModPointer[i];
                            current_phase = std::fmod(current_phase, TAU);
                            if (resetPhaseAt.count(i)) current_phase = 0;

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }
                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
                            current_phase += freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);
                            if (resetPhaseAt.count(i)) current_phase = 0;

                            if constexpr(waveInputType == waveType::sine) {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }
                    }
                }
            } else { // floating frequency, value from freq.

                if constexpr(phaseType == boolType::Floating)
                {// phase is just now an offset

                    current_phase = current_phase + phase;

                    if constexpr(amplType == boolType::Floating)
                    {

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                            current_phase += freq * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine)
                            {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }

                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                            current_phase += freq * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine)
                            {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }

                    }

                    current_phase = current_phase -
                                    phase;// if this is not done the phase offset keeps on increasing for every process call.
                } else {// here phase is a changing offset.
                    const float *phaseModPointer = phaseModBuffer->getReadPointer(channel);

                    if constexpr(amplType == boolType::Floating)
                    {

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                            current_phase += (freq * phaseIncrementRatio) + phaseModPointer[i];
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine)
                            {
                                channelData[i] = std::sin(current_phase) * amplitude;
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplitude;
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplitude;
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplitude;
                            }

                        }

                    } else {
                        const float *amplModPointer = amplitudeModBuffer->getReadPointer(channel);

                        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {

                            current_phase += freq * phaseIncrementRatio;
                            current_phase = std::fmod(current_phase, TAU);

                            if constexpr(waveInputType == waveType::sine)
                            {
                                channelData[i] = std::sin(current_phase) * amplModPointer[i];
                            } else if (waveInputType == waveType::sawtooth) {
                                channelData[i] = ((current_phase / PI) - 1) * amplModPointer[i];
                            } else if (waveInputType == waveType::triangle) {
                                float normalizedPhase = current_phase / TAU; // Normalize phase to [0, 1]
                                float triangle = 2.0f * std::abs(2.0f * (normalizedPhase - std::floor(normalizedPhase + 0.5f))) - 1.0f;
                                channelData[i] = triangle * amplModPointer[i];
                            } else {
                                float square = (current_phase < PI) ? 1.0f : -1.0f;
                                channelData[i] = square * amplModPointer[i];
                            }

                        }

                    }
                }
            }

            // update for the next block.
            phase_set[channel] = current_phase;
        }
    }


    void processGraphNode() override {

        float freq = InputSockets[1]->getValue();
        float ampli = InputSockets[2]->getValue();
        float phase = InputSockets[3]->getValue();

        (this->*callBackFunction)(freq, ampli, phase);

    }


    void releaseResources() override {}

    void reset() override {

        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        freqAtSamplePoint_from_MIDI.resize(estimatedSamplesPerBlock);

        frequencyModBuffer = InputSockets[1]->getBufferPointer();
        midiBuffer = InputSockets[1]->getMidiMessage();

        amplitudeModBuffer = InputSockets[2]->getBufferPointer();
        phaseModBuffer = InputSockets[3]->getBufferPointer();

        phaseIncrementRatio = (2.0 * PI) / sampleRate;

        phase_set[0] = 0.0;
        phase_set[1] = 0.0;

        presMIDI_freq = 0;
        presMIDI_Note = 0;
        MIDI_order.clear();

        setCallBackHell();

    }

    void mini_reset() override {
        setCallBackHell();
    }

    void setCallBackHell() {
        // I had the dumbest idea to make freqInputType even after having SocketDataType,
        // now here is my punishment.
        if (InputSockets[1]->getConnectionType() == SocketDataType::MIDI) {
            // select the amplitude type
            if (InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                // select the phase type
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::MIDI, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::MIDI, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::MIDI, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::MIDI, boolType::AudioBuffer, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::MIDI, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::MIDI, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::MIDI, boolType::AudioBuffer, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::MIDI, boolType::AudioBuffer, boolType::Floating>;
                }
            } else {
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::MIDI, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::MIDI, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::MIDI, boolType::Floating, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::MIDI, boolType::Floating, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::MIDI, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::MIDI, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::MIDI, boolType::Floating, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::MIDI, boolType::Floating, boolType::Floating>;
                }
            }
        } else if (InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            if (InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                // select the phase type
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::AudioBuffer, boolType::AudioBuffer, boolType::Floating>;
                }
            } else {
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::AudioBuffer, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::AudioBuffer, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::AudioBuffer, boolType::Floating, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::AudioBuffer, boolType::Floating, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::AudioBuffer, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::AudioBuffer, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::AudioBuffer, boolType::Floating, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::AudioBuffer, boolType::Floating, boolType::Floating>;
                }
            }
        } else {
            if (InputSockets[2]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                // select the phase type
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::Floating, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::Floating, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::Floating, boolType::AudioBuffer, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::Floating, boolType::AudioBuffer, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::Floating, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::Floating, boolType::AudioBuffer, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::Floating, boolType::AudioBuffer, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::Floating, boolType::AudioBuffer, boolType::Floating>;
                }
            } else {
                if (InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat) {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::Floating, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::Floating, boolType::Floating, boolType::AudioBuffer>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::Floating, boolType::Floating, boolType::AudioBuffer>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::Floating, boolType::Floating, boolType::AudioBuffer>;
                } else {
                    int wave_type_here = (int)InputSockets[0]->getValue();
                    if (wave_type_here == 1) callBackFunction = &Oscillator::generateProcessCallback< waveType::sine, freqInputType::Floating, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 2) callBackFunction = &Oscillator::generateProcessCallback< waveType::square, freqInputType::Floating, boolType::Floating, boolType::Floating>;
                    else if (wave_type_here == 3) callBackFunction = &Oscillator::generateProcessCallback< waveType::triangle, freqInputType::Floating, boolType::Floating, boolType::Floating>;
                    else callBackFunction = &Oscillator::generateProcessCallback< waveType::sawtooth, freqInputType::Floating, boolType::Floating, boolType::Floating>;
                }
            }
        }
    }

    ~Oscillator() {};

private:

    SocketDataType frequencyInputType = SocketDataType::NULLType;

    juce::MidiBuffer* midiBuffer;

    float phaseIncrementRatio = 0.0;

    float phase_set[2] = { 0.0, 0.0 };

    juce::AudioBuffer<float>* frequencyModBuffer, *amplitudeModBuffer, *phaseModBuffer;

    std::vector<float> freqAtSamplePoint_from_MIDI;

    void (Oscillator::*callBackFunction)( float, float, float );

    int presMIDI_Note = 0;
    float presMIDI_freq = 0;
    std::set<int> MIDI_order;


};