#include "MainComponent.h"
#include "MenuComponent.h"

MainComponent::MainComponent() {
    setSize (1200, 800);

    this->menuBarComponent = std::make_unique<MenuComponent>();
    addAndMakeVisible(menuBarComponent.get());


}


void MainComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::grey);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
}

void MainComponent::resized() {
    if (this->menuBarComponent != nullptr)
        this->menuBarComponent->setBounds(0, 0, getWidth(), 20);
}