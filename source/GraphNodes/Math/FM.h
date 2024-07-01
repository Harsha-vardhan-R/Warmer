/*
  ==============================================================================

    FM.h
    Created: 23 May 2024 3:42:21am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"

class FM : public GraphNode {
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

    FM(int pos_x, int pos_y) : GraphNode(juce::String("FM Operator"), pos_x, pos_y) {

        // Output wave.
        OutputSockets.add(new GraphNode::Socket(juce::String("Signal OUT"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Carrier Freq"), direction::IN, false));
        InputSockets[0]->acceptType(SocketDataType::Floating);
        InputSockets[0]->acceptType(SocketDataType::MIDI);
        InputSockets[0]->addSliderParameterControl(0.0, 10000.0, 440.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Harmonic"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.0, 10.0, 1.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Modulator"), direction::IN, true));
        InputSockets[2]->acceptType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Modulation Index"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[3]->addSliderParameterControl(0.0, 10.0, 0.5);

        InputSockets.add(new GraphNode::Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[4]->acceptType(SocketDataType::Floating);
        InputSockets[4]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[4]->addSliderParameterControl(0.0, 1.0, 0.4);

        makeAllSocketsVisible();
        resized();
    }

    void wrapPhase(float& phase) {
        if (phase >= TAU) phase -= TAU;
        else if (phase < 0) phase += TAU;
    }

    template< bool midiInput, bool modulatingHarmonics, bool modulatingModulationIndex, bool modulatingAmplitude>
    void generateSubProcess() {
        int numSamples = bufferToWritePointer->getNumSamples();

        float temp;

        for (int channel = 0; channel < 2; channel++) {
            float* channelData = bufferToWritePointer->getWritePointer(channel);
            const float* modulatorChannel = modulator->getReadPointer(channel);
            float local_carrier_phase = carrierPhase;

            if constexpr (midiInput) {
                juce::MidiMessage message;
                int samplePosition;

                int currentIndexPosition = 0;

                // Iterate through all the messages in the buffer
                for (const auto metadata : *midiBuffer) {
                    message = metadata.getMessage();
                    samplePosition = metadata.samplePosition;

                    // fill the `freqAtSamplePoint_from_MIDI` till here.
                    // no need to check for less than number of samples because `samplePosition` is always less than the number of samples.
                    for (; currentIndexPosition < samplePosition; ++currentIndexPosition) freqAtSamplePoint_from_MIDI[currentIndexPosition] = presMIDI_freq;

                    int note = std::clamp(message.getNoteNumber(), 0, 127); // makes sure we do not access outside the `midiFrequencies[128]`

                    // Check if the message is a Note On or Note Off message
                    if (message.isNoteOn()) {
                        presMIDI_Note = note;
                        presMIDI_freq = midiFrequencies[note];
                    }

                }

                // Fill remaining samples at the end in `freqAtSamplePoint_from_MIDI`
                for (; currentIndexPosition < bufferToWritePointer->getNumSamples(); ++currentIndexPosition) freqAtSamplePoint_from_MIDI[currentIndexPosition] = presMIDI_freq;

                if constexpr (modulatingHarmonics) {
                    const float* harmonics_mod_channel = harmonic_mod_buffer->getReadPointer(channel);

                    if constexpr (modulatingModulationIndex) {
                        const float* modulationIndex_mod_channel = modulating_index_mod_buffer->getReadPointer(channel);

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);

                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);

                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    } else {
                        float modulationIndex = InputSockets[3]->getValue();

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }
                    }

                } else {
                    float harmonic = InputSockets[1]->getValue();

                    if constexpr (modulatingModulationIndex) {
                        const float* modulationIndex_mod_channel = modulating_index_mod_buffer->getReadPointer(channel);

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonic;
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonic;
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    } else {
                        float modulationIndex = InputSockets[3]->getValue();

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonic;
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freqAtSamplePoint_from_MIDI[i] * phaseIncrementRatio * harmonic;
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    }
                }
            } else {
                float carrierFreq = InputSockets[0]->getValue();

                if constexpr (modulatingHarmonics) {

                    const float* harmonics_mod_channel = harmonic_mod_buffer->getReadPointer(channel);
                    float freq_times_phase_ratio = carrierFreq * phaseIncrementRatio;

                    if constexpr (modulatingModulationIndex) {
                        const float* modulationIndex_mod_channel = modulating_index_mod_buffer->getReadPointer(channel);

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freq_times_phase_ratio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freq_times_phase_ratio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    } else {
                        float modulationIndex = InputSockets[3]->getValue();

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freq_times_phase_ratio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float phaseIncrement = freq_times_phase_ratio * harmonics_mod_channel[i];
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    }
                } else {
                    float harmonic = InputSockets[1]->getValue();
                    float phaseIncrement = carrierFreq * phaseIncrementRatio * harmonic;

                    if constexpr (modulatingModulationIndex) {
                        const float* modulationIndex_mod_channel = modulating_index_mod_buffer->getReadPointer(channel);

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float resultantPhase = local_carrier_phase + modulationIndex_mod_channel[i] * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }

                    } else {
                        float modulationIndex = InputSockets[3]->getValue();

                        if constexpr (modulatingAmplitude) {
                            const float *mod_amp_channel = modAmpBuffer->getReadPointer(channel);

                            for (int i = 0; i < numSamples; ++i) {
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * mod_amp_channel[i];
                            }
                        } else {
                            const float amp = InputSockets[4]->getValue();

                            for (int i = 0; i < numSamples; ++i) {
                                float resultantPhase = local_carrier_phase + modulationIndex * modulatorChannel[i] * TAU;

                                wrapPhase(resultantPhase);

                                local_carrier_phase += phaseIncrement;
                                wrapPhase(local_carrier_phase);


                                channelData[i] = std::sin(resultantPhase) * amp;
                            }
                        }


                    }
                }
            }

            temp = local_carrier_phase;
        }

        carrierPhase = temp;
    }



    void processGraphNode() override {
        (this->*CallbackFunction)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);


        midiBuffer = InputSockets[0]->getMidiBuffer();
        harmonic_mod_buffer = InputSockets[1]->getBufferPointer();
        modulator = InputSockets[2]->getBufferPointer();
        modulating_index_mod_buffer = InputSockets[3]->getBufferPointer();
        modAmpBuffer = InputSockets[4]->getBufferPointer();


        phaseIncrementRatio = TAU / sampleRate;
        carrierPhase = 0.0f;


        freqAtSamplePoint_from_MIDI.resize(estimatedSamplesPerBlock);

        bool is_midi = InputSockets[0]->getConnectionType() == SocketDataType::MIDI;
        bool harmonics_modulating = InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat;
        bool index_modulating = InputSockets[3]->getConnectionType() == SocketDataType::AudioBufferFloat;
        bool ampl_modulating = InputSockets[4]->getConnectionType() == SocketDataType::AudioBufferFloat;

        if (is_midi) {
            if (harmonics_modulating && index_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<true, true, true, true>;
                else CallbackFunction = &FM::generateSubProcess<true, true, true, false>;
            } else if (harmonics_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<true, true, false, true>;
                else CallbackFunction = &FM::generateSubProcess<true, true, false, false>;
            } else if (index_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<true, false, true, true>;
                else CallbackFunction = &FM::generateSubProcess<true, false, true, false>;
            } else {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<true, false, false, true>;
                else CallbackFunction = &FM::generateSubProcess<true, false, false, false>;
            }
        } else {
            if (harmonics_modulating && index_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<false, true, true, true>;
                else CallbackFunction = &FM::generateSubProcess<false, true, true, false>;
            } else if (harmonics_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<false, true, false, true>;
                else CallbackFunction = &FM::generateSubProcess<false, true, false, false>;
            } else if (index_modulating) {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<false, false, true, true>;
                else CallbackFunction = &FM::generateSubProcess<false, false, true, false>;
            } else {
                if (ampl_modulating)  CallbackFunction = &FM::generateSubProcess<false, false, false, true>;
                else CallbackFunction = &FM::generateSubProcess<false, false, false, false>;
            }
        }

        presMIDI_Note = 0;
        presMIDI_freq = 0;
    }

    ~FM() {};

private:

    float carrierPhase = 0.0f;
    juce::AudioBuffer<float>* modulator = nullptr, *modAmpBuffer = nullptr;
    float phaseIncrementRatio = 0.0f;

    juce::MidiBuffer* midiBuffer = nullptr;

    std::vector<float> freqAtSamplePoint_from_MIDI;

    void (FM::*CallbackFunction)();

    juce::AudioBuffer<float> *harmonic_mod_buffer = nullptr, *modulating_index_mod_buffer = nullptr;

    int presMIDI_Note = 0;
    float presMIDI_freq = 0;
};