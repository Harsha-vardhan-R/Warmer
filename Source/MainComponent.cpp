#include "MainComponent.h"
#include "MenuComponent.h"
#include "MasterComponent.h"

MainComponent::MainComponent() {
    setSize (1500, 1000);

    this->menuBarComponent = std::make_unique<MenuComponent>();
    addAndMakeVisible(menuBarComponent.get());

    this->masterComponent = std::make_unique<MasterComponent>(menuBarComponent.get()->getHeight());
    addAndMakeVisible(masterComponent.get());

}


void MainComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::darkgrey);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);

}

void MainComponent::resized() {
    if (this->menuBarComponent != nullptr)
        this->menuBarComponent->setBounds(0, 0, getWidth(), 20);
}