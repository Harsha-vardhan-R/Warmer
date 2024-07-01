/*
  ==============================================================================

    RandomOscillator.h
    Created: 12 May 2024 7:20:28pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <climits>

class RandomOscillator : public GraphNode {
public:

    typedef void (RandomOscillator::*callbackFunction_type)(float);

    RandomOscillator(int pos_x, int pos_y) : GraphNode(juce::String("Random Oscillator"), pos_x, pos_y) {

        OutputSockets.add(new GraphNode::Socket(juce::String("Signal"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Interpolation"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("Robot");
        InputSockets[0]->addMenuItem("Sin");
        InputSockets[0]->addMenuItem("Linear");

        InputSockets.add(new GraphNode::Socket(juce::String("Refresh(ms)"), direction::IN, false));
        InputSockets[1]->acceptType(SocketDataType::Floating);
        InputSockets[1]->acceptType(SocketDataType::AudioBufferFloat);
        InputSockets[1]->addSliderParameterControl(0.001, 5000.0, 50.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Stereo"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(0.0, 1.0, 0.0);

        InputSockets.add(new GraphNode::Socket(juce::String("Amplitude"), direction::IN, false));
        InputSockets[3]->acceptType(SocketDataType::Floating);
        InputSockets[3]->addSliderParameterControl(0.0, 15.0, 0.4);

        makeAllSocketsVisible();

        resized();
    }

    enum class InterpolationType {
        robot,
        sin,
        linear
    };

    float sineInterpolation(float y1, float y2, float t) {
        // Adjust the sine interpolation to ensure continuous derivative at endpoints
        float A = (y2 - y1) / 2;
        float B = (y2 + y1) / 2;
        return A * std::sin(PI * t - PI / 2) + B;
    }

    template <InterpolationType interType, bool isRefreshModulating>
    void generateCallback(float amplitude) {

        float* channelData_left = bufferToWritePointer->getWritePointer(0);
        float* channelData_right = bufferToWritePointer->getWritePointer(1);
        int size_samples = bufferToWritePointer->getNumSamples();
        float stereo_separation = InputSockets[2]->getValue();


        if constexpr(isRefreshModulating) {
            const float* refreshData_left = refresh_mod->getReadPointer(0);
            const float* refreshData_right = refresh_mod->getReadPointer(1);

            int i = 0;

            while (i < size_samples) {
                float refresh_rate = (refreshData_left[i] + refreshData_right[i]) / 2.0f;
                int refresh_number_of_samples = std::clamp(static_cast<int>((refresh_rate / 1000.0f) * sampleRate), 1, INT_MAX);

                if (refresh_sample == 0) {
                    refresh_sample = refresh_number_of_samples;
                    float new_random = random.nextFloat() * 2.0f - 1.0f;
                    pres_random[0] = next_random[0]; pres_random[1] = next_random[1];
                    next_random[0] = ((random.nextFloat() * 2.0f - 1.0f) * stereo_separation) + new_random;
                    next_random[1] = ((random.nextFloat() * 2.0f - 1.0f) * stereo_separation) + new_random;
                }

                float lin_coeff_left = (next_random[0] - pres_random[0]) / static_cast<float>(refresh_number_of_samples);
                float lin_coeff_right = (next_random[1] - pres_random[1]) / static_cast<float>(refresh_number_of_samples);

                if constexpr (interType == InterpolationType::robot) {
                    channelData_left[i] = pres_random[0] * amplitude;
                    channelData_right[i] = pres_random[1] * amplitude;
                } else if (interType == InterpolationType::sin) {
                    float t = static_cast<float>(refresh_number_of_samples - refresh_sample) / static_cast<float>(refresh_number_of_samples);
                    channelData_left[i] = sineInterpolation(pres_random[0], next_random[0], t) * amplitude;
                    channelData_right[i] = sineInterpolation(pres_random[1], next_random[1], t) * amplitude;
                } else { // linear
                    channelData_left[i] = ((lin_coeff_left * (refresh_number_of_samples - refresh_sample)) + pres_random[0]) * amplitude;
                    channelData_right[i] = ((lin_coeff_right * (refresh_number_of_samples - refresh_sample)) + pres_random[1]) * amplitude;
                }

                refresh_sample--;
                i++;
            }
        } else { // refresh is constant for each block
            float refresh_rate = InputSockets[1]->getValue();
            int refresh_number_of_samples = std::clamp(static_cast<int>((refresh_rate / 1000.0f) * sampleRate), 1, INT_MAX);

            float lin_coeff_left = (next_random[0] - pres_random[0]) / static_cast<float>(refresh_number_of_samples);
            float lin_coeff_right = (next_random[1] - pres_random[1]) / static_cast<float>(refresh_number_of_samples);

            int i = 0;

            while (i < size_samples) {

                if (refresh_sample == 0) {
                    refresh_sample = refresh_number_of_samples;
                    float new_random = random.nextFloat() * 2.0f - 1.0f;
                    pres_random[0] = next_random[0]; pres_random[1] = next_random[1];
                    next_random[0] = ((random.nextFloat() * 2.0f - 1.0f) * stereo_separation) + new_random;
                    next_random[1] = ((random.nextFloat() * 2.0f - 1.0f) * stereo_separation) + new_random;
                    lin_coeff_left = (next_random[0] - pres_random[0]) / static_cast<float>(refresh_number_of_samples);
                    lin_coeff_right = (next_random[1] - pres_random[1]) / static_cast<float>(refresh_number_of_samples);
                }

                if constexpr (interType == InterpolationType::robot) {
                    channelData_left[i] = pres_random[0] * amplitude;
                    channelData_right[i] = pres_random[1] * amplitude;
                } else if (interType == InterpolationType::sin) {
                    float t = static_cast<float>(refresh_number_of_samples - refresh_sample) / static_cast<float>(refresh_number_of_samples);
                    channelData_left[i] = sineInterpolation(pres_random[0], next_random[0], t) * amplitude;
                    channelData_right[i] = sineInterpolation(pres_random[1], next_random[1], t) * amplitude;
                } else { // linear
                    channelData_left[i] = ((lin_coeff_left * (refresh_number_of_samples - refresh_sample)) + pres_random[0]) * amplitude;
                    channelData_right[i] = ((lin_coeff_right * (refresh_number_of_samples - refresh_sample)) + pres_random[1]) * amplitude;
                }

                refresh_sample--;
                i++;
            }

        }

    }


    void processGraphNode() override {
        float gain = InputSockets[3]->getValue();
        callbackFunction_type callback = callbackFunction.load();
        (this->*callback)(gain);
    }


    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);

        refresh_sample = 0;
        refresh_mod = InputSockets[1]->getBufferPointer();

        callbackFunction_type callback;

        int selected = (int)InputSockets[0]->getValue();

        if (InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            if (selected == 1) {
                callback = &RandomOscillator::generateCallback<InterpolationType::robot, true>;
            } else if (selected == 2) {
                callback = &RandomOscillator::generateCallback<InterpolationType::sin, true>;
            } else {
                callback = &RandomOscillator::generateCallback<InterpolationType::linear, true>;
            }
        } else {
            if (selected == 1) {
                callback = &RandomOscillator::generateCallback<InterpolationType::robot, false>;
            } else if (selected == 2) {
                callback = &RandomOscillator::generateCallback<InterpolationType::sin, false>;
            } else {
                callback = &RandomOscillator::generateCallback<InterpolationType::linear, false>;
            }
        }

        callbackFunction.store(callback);

    }

    void mini_reset() override {
        callbackFunction_type callback;

        int selected = (int)InputSockets[0]->getValue();

        if (InputSockets[1]->getConnectionType() == SocketDataType::AudioBufferFloat) {
            if (selected == 1) {
                callback = &RandomOscillator::generateCallback<InterpolationType::robot, true>;
            } else if (selected == 2) {
                callback = &RandomOscillator::generateCallback<InterpolationType::sin, true>;
            } else {
                callback = &RandomOscillator::generateCallback<InterpolationType::linear, true>;
            }
        } else {
            if (selected == 1) {
                callback = &RandomOscillator::generateCallback<InterpolationType::robot, false>;
            } else if (selected == 2) {
                callback = &RandomOscillator::generateCallback<InterpolationType::sin, false>;
            } else {
                callback = &RandomOscillator::generateCallback<InterpolationType::linear, false>;
            }
        }

        callbackFunction.store(callback);
    }

    ~RandomOscillator() {};

private:

    juce::Random random;

    float pres_random[2] = {0.0f, 0.0f}, next_random[2] = {0.0f, 0.0f};

    int refresh_sample = 0;

    std::atomic<callbackFunction_type> callbackFunction;

    juce::AudioBuffer<float> *refresh_mod = nullptr;

};