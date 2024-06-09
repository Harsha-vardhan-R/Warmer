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
#include "MenuComponent.h"

class look_and_feel_exp : public juce::LookAndFeel_V3 {
public:
    look_and_feel_exp() {
        setColour(juce::TabbedComponent::backgroundColourId, juce::Colours::white);
        setColour(juce::TabbedComponent::outlineColourId, juce::Colours::grey);
    }
};


/*
    Contains the code to manage the bar and component that is used to switch between modes, like `Play`, `Graph` and `Edit` mode.
    This component lies below `instrument-And-PresetSelector`
*/
class ModeSwitch final : public juce::TabbedComponent {
public:

    look_and_feel_exp s;

    ModeSwitch() : instrument(30),
                    juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtLeft) {

        auto colour_here = juce::Colours::white;

        this->setTabBarDepth(30);

        addTab("Play", colour_here, instrument.getPlayPage() , true);
        addTab("Graph", colour_here, instrument.getGraphPage() , true);// viewport is set as the component.
        addTab("Edit", colour_here, instrument.getEditPage() , true);

        setCurrentTabIndex(1, false);
//        styles.reset(new MenuComponent());
//        setLookAndFeel(styles.get());

        setLookAndFeel(&s);

        instrumentPresent = false;

        resized();
    }

    ~ModeSwitch() override {
        setLookAndFeel(nullptr);
    }
    //+++++++++++++++
    void resized() override {
        setBounds(0, 35, getParentWidth(), getParentHeight()-35);
        juce::TabbedComponent::resized();
    }
    //==============
    // Called when the tab is switched, this will call the instrument's setMode()
    void currentTabChanged(int newTabIndex, const juce::String& newTabName) override {
        switch (newTabIndex) {
            case 0:
                instrument.setMode(Mode::Play);
                break;
            case 1:
                instrument.setMode(Mode::Graph);
                break;
            case 2:
                instrument.setMode(Mode::Edit);
                break;
        }
    }


private:
    Instrument instrument;

    bool instrumentPresent;
    std::unique_ptr<juce::LookAndFeel_V4> styles;

    const std::vector<std::string> modeTypes = { "Play",
                                                 "Graph",
                                                 "Edit" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeSwitch)
};