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


/*
    Contains the code to manage the bar and component that is used to switch between modes, like `Play`, `Graph` and `Edit` mode.
    This component lies below `instrumentAndPresetSelector`
*/
class ModeSwitch final : public juce::TabbedComponent {
public:

    ModeSwitch() : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtLeft) {
        //setSize(getParentWidth(), getParentHeight());
        auto colour_here = juce::Colours::white;

        this->setTabBarDepth(30);
        this->instrument = std::make_unique<Instrument>(getTabBarDepth());

        addTab("Play", colour_here, this->instrument.get()->getPlayPage() , true);
        addTab("Graph", colour_here, this->instrument.get()->getGraphPage() , true);// viewport is set as the component.
        addTab("Edit", colour_here, this->instrument.get()->getEditPage() , true);

        styles.reset(new MenuComponent());
        setLookAndFeel(styles.get());

        instrumentPresent = false;

        resized();
    }

    ~ModeSwitch() override {}
    //+++++++++++++++
    void resized() override {
        setBounds(0, 35, getParentWidth(), getParentHeight()-35);
        juce::TabbedComponent::resized();
    }
    //==============
    // Called when the tab is switched, this will call the instrument's setMode()
    void currentTabChanged(int newTabIndex, const juce::String& newTabName) override {
        if (instrument.get() == nullptr) return ;

        switch (newTabIndex) {
            case 0:
                instrument.get()->setMode(Mode::Play);
                break;
            case 1:
                instrument.get()->setMode(Mode::Graph);
                break;
            case 2:
                instrument.get()->setMode(Mode::Edit);
                break;
        }
    }


private:
    std::unique_ptr<Instrument> instrument;
    bool instrumentPresent;
    std::unique_ptr<juce::LookAndFeel_V4> styles;

    const std::vector<std::string> modeTypes = { "Play",
                                                 "Graph",
                                                 "Edit" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeSwitch)
};