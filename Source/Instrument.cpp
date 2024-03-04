/*
  ==============================================================================

    Instrument.cpp
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/


#include "Instrument.h"
#include "MyLookAndFeel.h"
#include "Modes.h"
#include "./GraphNodes/Collection.h"
#include "Piano.h"
#include "Wheels.h"
#include "ColourPalette.h"
#include <JuceHeader.h>

Instrument* Instrument::instancePtr = nullptr;

void* Instrument::VoidPointerToWheelComponent = nullptr;
void* Instrument::VoidPointerToPianoComponent = nullptr;

// This maps from the midi input to my representation,
// anything above 99 is for a black key, whose actual value is (value-100)
int MIDI_TO_MINE[113] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

        0, 100, 1, 101, 2, 3, 102, 4, 103, 5, 104, 6,
        7, 105, 8, 106, 9, 10, 107, 11, 108, 12, 109, 13,
        14, 110, 15, 111, 16, 17, 112, 18, 113, 19, 114, 20,
        21, 115, 22, 116, 23, 24, 117, 25, 118, 26, 119, 27,
        28, 120, 29, 121, 30, 31, 122, 32, 123, 33, 124, 34,
        35, 125, 36, 126, 37, 38, 127, 39, 128, 40, 129, 41,
        42, 130, 43, 131, 44, 45, 132, 46, 133, 47, 134, 48,
        48, 135, 49, 136, 50
};


Instrument::Instrument(int tabWidth) {


    this->tabWidth = tabWidth;

    editPage.reset(new Instrument::EditPage());
    graphPage.reset(new Instrument::GraphPage());
    playPage.reset(new Instrument::PlayPage());

    presentMode = Mode::Play;


    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    // Create and add MidiInput objects to the owned array
    for (const auto& device : midiDevicesHere) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        if (midiInput != nullptr) {
            midiInput->start();
            midiInputs.add(std::move(midiInput));
        } else {
            std::cerr << "Failed to open MIDI device: " << device.identifier << std::endl;
        }
    }

    std::cout << "Available Devices : " << "\n";
    for (auto i : midiInputs) {
        std::cout << i->getName() << "\n";
    }

    deviceManager.reset(new juce::AudioDeviceManager());
    deviceManager.get()->initialise(2, 2, nullptr, true);

    AudioMIDISettingsJUCE = std::make_unique<Instrument::AudioMIDISettingClass>(*deviceManager.get());
    AudioMIDISettingsJUCE.get()->closeButtonPressed();


    resized();

    Instrument::instancePtr = this;
}


// TODO : ask to save changes, before replacing.
void Instrument::Initialize() {
    // Just replacing all the changed pages to new ones.
//    editPage.get() = *(new Instrument::EditPage());
//    graphPage.get() = *(new Instrument::GraphPage());
//    playPage.get() = *(new Instrument::PlayPage());
}

Instrument::~Instrument() {

}

void Instrument::paint(juce::Graphics& g) {}


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

    if (AudioMIDISettingsJUCE.get() != nullptr) AudioMIDISettingsJUCE.get()->setBounds(0, 0, 300, 300);
}


/////////////////////////////
/////// EDIT PAGE ///////////
/////////////////////////////
Instrument::EditPage::EditPage() {

    size_x.reset(new juce::TextEditor());
    addAndMakeVisible(size_x.get());

    size_y.reset(new juce::TextEditor());
    addAndMakeVisible(size_y.get());

    createCanvasButton.reset(new juce::TextButton());
    createCanvasButton.get()->setButtonText("Create Instrument Canvas");
    addAndMakeVisible(createCanvasButton.get());
    createCanvasButton.get()->onClick = [this] { createCanvasButtonClicked(); };


    resized();
}

void Instrument::EditPage::resized() {

    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30, 1, parent->getWidth() - 29, parent->getHeight() - 2);


    auto bounds = getLocalBounds();
    auto horizontalCenter = bounds.getX()+getHeight()/2;
    auto verticalCenter = bounds.getX()+getWidth()/2;

    if (size_x.get() != nullptr) size_x.get()->setBounds(verticalCenter-180, horizontalCenter, 50, 25);
    if (size_y.get() != nullptr) size_y.get()->setBounds(verticalCenter-125, horizontalCenter, 50, 25);
    if (createCanvasButton.get() != nullptr) createCanvasButton.get()->setBounds(verticalCenter-65, horizontalCenter, 200, 25);

    if (Canvas.get() != nullptr) Canvas.get()->resized();

}

void Instrument::EditPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::white);
}

void Instrument::EditPage::createCanvasButtonClicked() {
    juce::String x_dimen = size_x.get()->getText();
    juce::String y_dimen = size_y.get()->getText();

    int x = x_dimen.getIntValue();
    int y = y_dimen.getIntValue();

    if (x_dimen.isNotEmpty() && y_dimen.isNotEmpty()) {
        if (x != 0 && y != 0) {
            if ((x > 100 && x < 2500) && (y > 100 && y < 2500)) { // Create the canvas.
                this->x = x;
                this->y = y;
                Canvas.reset(new InstrumentCanvas(x, y));
                // Deleting the components and adding the canvas.
                size_x.reset(nullptr);
                size_y.reset(nullptr);
                createCanvasButton.reset(nullptr);

                addChildComponent(Canvas.get());
                addAndMakeVisible(Canvas.get());
                resized();
                repaint();
            } else {
                std::cout << "The dimensions are too small or too large to create the canvas" << "\n";
                return;
            }
        } else {
            std::cout << "Invalid inputs to create the canvas" << "\n";
            return;
        }
    } else {
        std::cout << "Please fill both the fields to create the canvas" << "\n";
        return;
    }
}

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
void Instrument::EditPage::InstrumentCanvas::paint(juce::Graphics &g) {
    g.fillAll(InstrumentDefaultBackColourID);
}

//////////////////////////////
/////// GRAPH PAGE ///////////
//////////////////////////////
Instrument::GraphPage::GraphPage() {
    resized();
}

void Instrument::GraphPage::resized() {
    juce::TabbedComponent* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30, 1, parent->getWidth() - 29, parent->getHeight() - 2);
}

void Instrument::GraphPage::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::white);
}

/////////////////////////////
/////// VIEW PAGE ///////////
//////////////////////////////
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
    g.fillAll(juce::Colours::aqua);
}

void Instrument::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {

    // Lock the message because we are calling from a different thread(most probably).
    juce::MessageManagerLock _messageManagerLock;

    if (Instrument::VoidPointerToPianoComponent == nullptr) {
        return;
    }

    auto PianoComponent = (Piano*)Instrument::VoidPointerToPianoComponent;
    auto comp = (Wheels*)Instrument::VoidPointerToWheelComponent;

    if (message.isNoteOn()) {
        int noteNumber = message.getNoteNumber();
        int velocity = message.getVelocity(); // need to set.
        int midi_ = MIDI_TO_MINE[noteNumber];

        if (midi_ > 99) {
            PianoComponent->overlayPainter.get()->BlackKeyDown(midi_-100);
        } else if (midi_ > -1) {
            PianoComponent->overlayPainter.get()->WhiteKeyDown(midi_);
        }

    } else if (message.isNoteOff()) {
        int noteNumber = message.getNoteNumber();
        int midi_ = MIDI_TO_MINE[noteNumber];

        if (midi_ > 99) {
            PianoComponent->overlayPainter.get()->BlackKeyUp(midi_-100);
        } else if (midi_ > -1) {
            PianoComponent->overlayPainter.get()->WhiteKeyUp(midi_);
        }
    } else if (message.isPitchWheel() && std::rand()%15 == 0) {
        if (comp == nullptr) return;
        comp->setPitchWheel((float)(message.getPitchWheelValue()/128));
    }


    PianoComponent->overlayPainter.get()->repaint();
}



Instrument::AudioMIDISettingClass::AudioMIDISettingClass(juce::AudioDeviceManager& deviceManager) : DocumentWindow("Audio/MIDI Settings",
                                                                            ButtonOutlineColourID,
                                                                            DocumentWindow::closeButton) {

    // This is to make sure the input audio is set to none.
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup(setup);
    setup.inputDeviceName = "";
    setup.useDefaultInputChannels = false;
    setup.inputChannels.clear();
    deviceManager.setAudioDeviceSetup(setup, true);


    settingPage.reset(new juce::AudioDeviceSelectorComponent(deviceManager,0, 0, 0,256, true, false, true, false));
    settingPage.get()->setSize(1200, 800);
//    setResizable(true, true);
//    setDraggable(false);
//    setUsingNativeTitleBar(true);

    setContentOwned(settingPage.get(), true);
    //setVisible(false);

}

