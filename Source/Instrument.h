/*
  ==============================================================================

    Instrument.h
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Modes.h"
#include "Piano.h"


/*
    Contains all the functions related to Instrument.
    Everything from loading an instrument, saving it, playing it and painting it on the component.
*/
class Instrument : public juce::Component,
                   public juce::MidiInputCallback {
public:

    // Maintaining a global static variable for the instance pointer,
    // Not a good idea, but refactoring takes years.
    // no memory management required as it will be taken care of.
    static Instrument* instancePtr;


    static Instrument* getInstance() {

        if (Instrument::instancePtr != nullptr) {
            return instancePtr;
        }

        int *ptr = nullptr;
        int a;
        std::cout << "Instruments get instance is called before creating the function, CRASHING!!" << "\n";
        a = *ptr;
        return nullptr;// compiler complaining.
    }


    // Get instance for the api calls.
    static void* VoidPointerToPianoComponent;
    static void* VoidPointerToWheelComponent;

    Instrument(int);
    ~Instrument() override;
    //++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;
    //====== INSTRUMENT'S API's ========
    void OpenAudioAndMIDISettings() {
        AudioMIDISettingsJUCE.get()->setVisible(true);
        AudioMIDISettingsJUCE.get()->setCentrePosition(getParentWidth()/2, getParentHeight()/2);
    }

    void Initialize();

    // This is like a shortcut,
    // Turns ON all the non-NULL MIDI devices.
    void refreshMIDIDevices() {
        // Firstly remove all the devices.
        while (midiInputs.size() != 0) {
            midiInputs.remove(0);
        }

        // Setting up for the MIDI listening.
        auto midiDevicesHere = juce::MidiInput::getAvailableDevices();


        for (const auto& device : midiDevicesHere) {
            auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
            if (midiInput != nullptr) {
                midiInput->start();
                midiInputs.add(std::move(midiInput));
            } else {
                std::cerr << "Failed to open MIDI device: " << device.identifier << std::endl;
            }
        }

        std::cout << "Listening From : " << "\n";
        for (auto i : midiInputs) {
            std::cout << i->getName() << "\n";
        }
    }

    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

    /* Sets the mode and calls the paint function, basically will switch tabs */
    void setMode(Mode mode);

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class EditPage : public juce::Component {
    public:

        EditPage();
        ~EditPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

        // Makes sure the numbers in the text boxes are valid,
        // Creates a canvas if they are.
        void createCanvasButtonClicked();

    private:

        class InstrumentCanvas : public juce::Component {
        public:
            InstrumentCanvas(int x, int y) {
                this->x = x;
                this->y = y;
            };
            ~InstrumentCanvas() override {};

            void paint(juce::Graphics& g) override;
            void resized() override {
                setBounds((getParentWidth()/2)-(x/2), (getParentHeight()/2)-(y/2), x, y);
            };

        private:
            int x, y;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentCanvas)
        };

        bool isCanvasPresent;
        std::unique_ptr<juce::Component> Canvas;

        int x, y;

        std::unique_ptr<juce::TextEditor> size_x;
        std::unique_ptr<juce::TextEditor> size_y;
        std::unique_ptr<juce::TextButton> createCanvasButton;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditPage)
    };

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class GraphPage : public juce::Component {
    public:
        GraphPage();
        ~GraphPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        std::unique_ptr<juce::PopupMenu> AddNodesPopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphPage)
    };

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class PlayPage : public juce::Component {
    public:
        PlayPage();
        ~PlayPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPage)
    };


    class AudioMIDISettingClass : public juce::DocumentWindow {
    public:
        AudioMIDISettingClass(juce::AudioDeviceManager&);
        ~AudioMIDISettingClass() override {}

        void closeButtonPressed() override {
            setVisible(false); // Hide the window when close button is pressed
        }

    private:
        std::unique_ptr<juce::Component> settingPage;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMIDISettingClass)
    };



    // Because the pages to be displayed are private, get functions for the pages.
    // Please do not use this before creating the pages, or else everything will crash
    juce::Component* getEditPage() {
        return this->editPage.get();
    }

    juce::Component* getGraphPage() {
        return this->graphPage.get();
    }

    juce::Component* getPlayPage() {
        return this->playPage.get();
    }



private:
    std::unique_ptr<juce::Component> editPage;
    std::unique_ptr<juce::Component> graphPage;
    std::unique_ptr<juce::Component> playPage;

    int size_width, size_height;
    int tabWidth;

    Mode presentMode;

    juce::OwnedArray<juce::MidiInput> midiInputs;
    std::unique_ptr<juce::DocumentWindow> AudioMIDISettingsJUCE;
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
