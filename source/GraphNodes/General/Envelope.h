/*
  ==============================================================================

    Envelope.h
    Created: 12 May 2024 11:27:08am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "../GraphNode.h"
#include <random>

class Envelope : public GraphNode {
public:

    typedef void (Envelope::*callbackFunction_type)();

    Envelope(int pos_x, int pos_y) : GraphNode(juce::String("Envelope"), pos_x, pos_y)  {

        OutputSockets.add(new GraphNode::Socket(juce::String("Envelope"), direction::OUT, true));
        OutputSockets[0]->setOutputType(SocketDataType::AudioBufferFloat);

        InputSockets.add(new GraphNode::Socket(juce::String("Trigger Type"), direction::IN, false));
        InputSockets[0]->addMenuParameterControl();
        InputSockets[0]->addMenuItem("Trigger"); // stops producing anything after note released.
        InputSockets[0]->addMenuItem("Trigger & Continue"); // goes till end unless no new note is
        InputSockets[0]->addMenuItem("Loop");
        InputSockets[0]->addMenuItem("Loop Continue");


        InputSockets.add(new GraphNode::Socket(juce::String("Trigger"), direction::IN, true));
        InputSockets[1]->acceptType(SocketDataType::MIDI);

        InputSockets.add(new GraphNode::Socket(juce::String("Duration(ms)"), direction::IN, false));
        InputSockets[2]->acceptType(SocketDataType::Floating);
        InputSockets[2]->addSliderParameterControl(1.0f, 16000.0, 120.0);

        InputSockets.add(new GraphNode::Socket(juce::String("      Envelope   "), direction::IN, false));
        InputSockets[3]->addEnvParameterControl();


        makeAllSocketsVisible();
        resized();

        envelope = static_cast<envParamCtrl*>(InputSockets[3]->getPointerToPresentParameterCtrlController());

        if (!envelope) std::cout << "Dayum, Did not work out! Envelope.h :48" << "\n";
    }


    template<int type>
    void subProcessGenerator() {

        if (needsReloading.load()) {
            envelope->copyData(points, controlLevels);
            needsReloading.store(false);
        }
        float* channelData = bufferToWritePointer->getWritePointer(0);

        juce::MidiMessage message;
        int samplePosition;

        int currentIndexPosition = 0;

        // Iterate through all the messages in the buffer
        for (const auto metadata : *midiBuffer) {
            message = metadata.getMessage();
            samplePosition = metadata.samplePosition;
            int note = std::clamp(message.getNoteNumber(), 0, 127);

            for (; currentIndexPosition < samplePosition; ++currentIndexPosition) {

                if constexpr (type == 1) {
                    if (presMIDI_Note != -1) {
                        channelData[currentIndexPosition] = getYForX(timeCoordinate);
                        timeCoordinate += timeCoordinateIncrement;
                    } else {
                        channelData[currentIndexPosition] = 0.0f;
                    }
                } else if (type == 2) {
                    // it keeps on increasing, after 1.0 we are goin to get the same last y value,
                    // which is what we want.
                    channelData[currentIndexPosition] = getYForX(timeCoordinate);
                    timeCoordinate += timeCoordinateIncrement;
                } else if (type == 3) {
                    // if some note is pressed run the loop else stop it.
                    if (presMIDI_Note != -1) {
                        channelData[currentIndexPosition] = getYForX(timeCoordinate);
                        timeCoordinate += timeCoordinateIncrement;
                        if (timeCoordinate > 1.0f) timeCoordinate -= 1.0f;
                    } else {
                        float targetLevel = 0.0;
                        channelData[currentIndexPosition] = getYForX(timeCoordinate);
                    }
                } else {
                    channelData[currentIndexPosition] = getYForX(timeCoordinate);
                    timeCoordinate += timeCoordinateIncrement;
                    if (timeCoordinate > 1.0f) timeCoordinate -= 1.0f;
                }

            }


            if (message.isNoteOn()) {
                presMIDI_Note = note;
                timeCoordinate = 0.0f; // reset time.
            }

            if (message.isNoteOff() && note == presMIDI_Note) {
                if constexpr (type != 2) presMIDI_Note = -1;
            }

        }


        for (; currentIndexPosition < bufferToWritePointer->getNumSamples(); ++currentIndexPosition) {

            if constexpr (type == 1) {
                if (presMIDI_Note != -1) {
                    channelData[currentIndexPosition] = getYForX(timeCoordinate);
                    timeCoordinate += timeCoordinateIncrement;
                } else {
                    channelData[currentIndexPosition] = 0.0f;
                }
            } else if (type == 2) {
                // it keeps on increasing, after 1.0 we are goin to get the same last y value,
                // which is what we want.
                channelData[currentIndexPosition] = getYForX(timeCoordinate);
                timeCoordinate += timeCoordinateIncrement;
            } else if (type == 3) {
                // if some note is pressed run the loop else stop it.
                if (presMIDI_Note != -1) {
                    channelData[currentIndexPosition] = getYForX(timeCoordinate);
                    timeCoordinate += timeCoordinateIncrement;
                    if (timeCoordinate > 1.0f) timeCoordinate -= 1.0f;
                } else {
                    float targetLevel = 0.0;
                    channelData[currentIndexPosition] = getYForX(timeCoordinate);
                }
            } else {
                channelData[currentIndexPosition] = getYForX(timeCoordinate);
                timeCoordinate += timeCoordinateIncrement;
                if (timeCoordinate > 1.0f) timeCoordinate -= 1.0f;
            }

        }

        // copy the stuff to the right channel.
        float *channelDataRight = bufferToWritePointer->getWritePointer(1);
        for (int i = 0; i < bufferToWritePointer->getNumSamples(); ++i) {
            channelDataRight[i] = channelData[i];
        }

    }


    void mini_reset() override {
        needsReloading.store(true);

        int option = (int)InputSockets[0]->getValue();

        if (presentSelectedCallback != option) {
            if (option == 1) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<1>);
            else if (option == 2) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<2>);
            else if (option == 3) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<3>);
            else callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<4>);
            presentSelectedCallback = (int)option;
        }

        presMIDI_Note = -1;
    }

    void processGraphNode() override {
        timeCoordinateIncrement = 1.0f / (std::clamp<float>(InputSockets[2]->getValue(), 1.0f, 16000.0f) * (float)sampleRate * 0.001f);

        callbackFunction_type callback = callBackFunctionEnvelope.load();
        (this->*callBackFunctionEnvelope)();
    }

    void releaseResources() override {}

    void reset() override {
        OutputSockets[0]->setBufferPointer(bufferToWritePointer);
        midiBuffer = InputSockets[1]->getMidiBuffer();

        needsReloading.store(true);

        presMIDI_Note = -1;

        int option = (int)InputSockets[0]->getValue();

        if (presentSelectedCallback != option) {
            if (option == 1) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<1>);
            else if (option == 2) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<2>);
            else if (option == 3) callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<3>);
            else callBackFunctionEnvelope.store(&Envelope::subProcessGenerator<4>);
            presentSelectedCallback = (int)option;
        }

    }

    ~Envelope() override = default;

private:
    std::atomic<callbackFunction_type> callBackFunctionEnvelope = nullptr;

    envParamCtrl* envelope = nullptr;

    std::vector<envParamCtrl::Point> points;
    std::vector<float> controlLevels;

    juce::MidiBuffer* midiBuffer = nullptr;

    inline float getYForX(float x) const {
        if (x < points.front().x) return 0.0;
        if (x >= points.back().x) return points.back().y;

        for (size_t i = 1; i < points.size(); ++i) {
            if (x < points[i].x) {
                float x1 = points[i - 1].x;
                float y1 = points[i - 1].y;
                float x2 = points[i].x;
                float y2 = points[i].y;

                // Calculate the control point
                float yControl = (y2 - y1) * controlLevels[i - 1] + y1;
                float xControl = (x2 - x1) * (1.0f - controlLevels[i - 1]) + x1;

                // Calculate the position on the quadratic Bezier curve
                float t = (x - x1) / (x2 - x1);

                float y = (1 - t) * (1 - t) * y1 + 2 * (1 - t) * t * yControl + t * t * y2;
                return y;
            }
        }

        return 0.0f;
    }

    // time from 0 -> 1
    // we just increment this based on the time and sample rate.
    float timeCoordinate = 0.0f;
    // how much does timeCoordinate should increment for each sample.
    float timeCoordinateIncrement = 0.0f;

    int presMIDI_Note = 0;

    std::atomic<bool> needsReloading;

    int presentSelectedCallback = -1;
};