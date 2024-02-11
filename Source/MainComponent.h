#pragma once

#include <JuceHeader.h>
#include "MenuComponent.h"

class MainComponent : public juce::Component {
public:
    MainComponent();
    ~MainComponent() override {};

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<MenuComponent> menuBarComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};