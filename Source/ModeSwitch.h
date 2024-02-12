/*
  ==============================================================================

    ModeSwitch.h
    Created: 12 Feb 2024 2:56:53am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
    Contains the code to manage the top bar that is used to switch between modes, like `Play`, `Graph`, `Design` and `Edit` mode.
*/
class ModeSwitch : public juce::Component {
public:
    enum Mode {
        Play; // Using the instrument.
        Graph; // Editing the node setup.
        Edit; // Editing the Appearance and the Position of elements on the instrument.
    };

    void paint(juce::Graphics& g) override;
    void resized() override;

};