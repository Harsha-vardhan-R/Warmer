/*
  ==============================================================================

    VolumeLevel.h
    Created: 29 Feb 2024 7:02:30pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>


class VolumeLevel : public juce::Component {
public :

    static inline VolumeLevel* instance = nullptr;

    VolumeLevel(int offset);
    ~VolumeLevel() override {};

    void paint(juce::Graphics& g) override;
    void resized() override;


    static void* getInstance();


private :

    int offset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeLevel)
};