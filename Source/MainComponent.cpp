#include "MainComponent.h"
#include "MenuComponent.h"
#include "MasterComponent.h"
#include "MIDIPianoComponent.h"

MainComponent::MainComponent() {
    setSize (1400, 850);
    this->minWidth = 400;
    this->minHeight = 400;

    this->menuBarComponent = std::make_unique<MenuComponent>();
    addAndMakeVisible(menuBarComponent.get());

    // Also takes the height of remaining part of the other two components combined take(because the menu and the piano's sizes are hardcoded).
    this->masterComponent = std::make_unique<MasterComponent>(menuBarComponent.get()->getHeight(), 150);
    addAndMakeVisible(masterComponent.get());

    // The height will be 'Total height of the MainWindow - Menu height(25px) - Master height(0.8 of MainWindow)'
    // The offset is the remaining part of the window height.
    this->midiPianoComponent = std::make_unique<MIDIPianoComponent>( getHeight()-125,
                                                                     125
    );
    addAndMakeVisible(midiPianoComponent.get());

    resized();

}

MainComponent::~MainComponent() {

}


void MainComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::darkgrey);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
}

void MainComponent::resized() {
    if (masterComponent.get() != nullptr) masterComponent.get()->resized();
    if (menuBarComponent.get() != nullptr) menuBarComponent.get()->resized();
    if (midiPianoComponent.get() != nullptr) midiPianoComponent.get()->resized();
}