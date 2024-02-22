/*
  ==============================================================================

    Instrument.cpp
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/


#include "Instrument.h"
#include "Modes.h"



Instrument::Instrument(int tabWidth) {
    this->tabWidth = tabWidth;

    editPage.reset(new Instrument::EditPage());
    graphPage.reset(new Instrument::GraphPage());
    playPage.reset(new Instrument::PlayPage());

    presentMode = Mode::Play;
    resized();
}

Instrument::~Instrument() {}

void Instrument::paint(juce::Graphics& g) {
      g.fillAll(juce::Colour(0xffeee3e7));
//
//    g.setColour(juce::Colour(0xfff6abb6));
//    switch (this->presentMode) {
//        case Mode::Play:
//            g.setColour(juce::Colours::grey);
//        case Mode::Edit:
//            g.setColour(juce::Colours::red);
//        case Mode::Graph:
//            g.setColour(juce::Colours::darksalmon);
//    }
}


void Instrument::setMode(Mode mode) {
    presentMode = mode;
    repaint();
}

void Instrument::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();

    if (parent != nullptr) setBounds(this->tabWidth,
                                     1,
                                     parent->getWidth() - this->tabWidth - 1,
                                     parent->getHeight() - 2);

    if (editPage.get()->getParentComponent() != nullptr) {editPage.get()->resized();}
    if (graphPage.get()->getParentComponent() != nullptr) {graphPage.get()->resized();}
    if (playPage.get()->getParentComponent() != nullptr) {playPage.get()->resized();}

}



/////// EDIT PAGE ///////////
Instrument::EditPage::EditPage() {
    resized();
}

void Instrument::EditPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30,
              1,
              parent->getWidth() - 29,
              parent->getHeight() - 2);
}

void Instrument::EditPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::green);
}



/////// GRAPH PAGE ///////////
Instrument::GraphPage::GraphPage() {
    resized();
}

void Instrument::GraphPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30,
              1,
              parent->getWidth() - 29,
              parent->getHeight() - 2);
}

void Instrument::GraphPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::blue);
}


/////// VIEW PAGE ///////////
Instrument::PlayPage::PlayPage() {
    resized();
}

void Instrument::PlayPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30,
              1,
              parent->getWidth() - 29,
              parent->getHeight() - 2);
}

void Instrument::PlayPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::red);
}

