/*
  ==============================================================================

    ModeSwitch.h
    Created: 12 Feb 2024 2:56:53am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Instrument.h"
#include "Modes.h"


/*
    Contains the code to manage the bar and component that is used to switch between modes, like `Play`, `Graph` and `Edit` mode.
    This component lies below `instrumentAndPresetSelector`
*/
class ModeSwitch final : public juce::TabbedComponent {
public:

    ModeSwitch() : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtLeft) {
        setSize(getParentWidth(), getParentHeight());
        auto colour_here = juce::Colour(0xff262b2b);

        this->instrument = std::make_unique<Instrument>(getTabBarDepth());

        addTab("Play", colour_here, this->instrument.get() , true);
        addTab("Graph", colour_here, this->instrument.get() , true);
        addTab("Edit", colour_here, this->instrument.get() , true);

        //this->addMouseListener(this);

        instrumentPresent = false;
    }

    ~ModeSwitch() override {
        instrument.get()->~Instrument();
    }
    //+++++++++++++++
    void resized() override {
        setBounds(0, 35, getParentWidth(), getParentHeight()-35);
        juce::TabbedComponent::resized();
    }
    //==============
    // Called when the tab is switched, this will call the instrument's setMode()



private:
    std::unique_ptr<Instrument> instrument;
    bool instrumentPresent;

    const std::vector<std::string> modeTypes = { "Play",
                                                 "Graph",
                                                 "Edit" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeSwitch)
};