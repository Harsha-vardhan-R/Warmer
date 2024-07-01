#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Instrument.h"
#include "MenuComponent.h"
#include "MasterComponent.h"
#include "MIDIPianoComponent.h"

class MainComponent : public juce::Component {
public:
    explicit MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    int minWidth, minHeight;

    std::unique_ptr<MenuComponent> menuBarComponent;
    std::unique_ptr<MasterComponent> masterComponent;
    std::unique_ptr<MIDIPianoComponent> midiPianoComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
