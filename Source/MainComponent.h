#pragma once

#include <JuceHeader.h>
#include "MenuComponent.h"
#include "MasterComponent.h"

class MainComponent : public juce::Component {
public:
    MainComponent();
    ~MainComponent() override {};

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<MenuComponent> menuBarComponent;
    std::unique_ptr<MasterComponent> masterComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};