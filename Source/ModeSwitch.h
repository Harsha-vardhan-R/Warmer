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
    Contains the code to manage the bar and component that is used to switch between modes, like `Play`, `Graph` and `Edit` mode.
    This component lies below `instrumentAndPresetSelector`
*/
class ModeSwitch final : public juce::TabbedComponent {
public:

    enum Mode {
        Play, // Using the instrument.
        Graph, // Editing the node setup.
        Edit, // Editing the Appearance and the Position of elements on the instrument.
    };

    ModeSwitch() : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtLeft) {
        auto colour_here = juce::Colour(0xff262b2b);

        addTab("Play", colour_here, new juce::Component() , false);
        addTab("Graph", colour_here, new juce::Component() , false);
        addTab("Edit", colour_here, new juce::Component() , false);
    }

    ~ModeSwitch() override {
        clearTabs(); /* Removes all the tabs before closing(No Memory Leaks) */
        deleteAllChildren();
    }


private:
    const std::vector<std::string> modeTypes = { "Play",
                                                 "Graph",
                                                 "Edit" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeSwitch)
};