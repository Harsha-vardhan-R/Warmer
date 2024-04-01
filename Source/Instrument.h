/*
  ==============================================================================

    Instrument.h
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <thread>
#include <atomic>

#include <JuceHeader.h>
#include "Modes.h"
#include "Piano.h"
#include "GraphNodes/Collection.h"
#include "MyDataStructures.h"


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
        AudioMIDISettingsJUCE.get()->setCentrePosition(getParentWidth()/2, getParentHeight()/2);
        AudioMIDISettingsJUCE.get()->setVisible(true);
    }

    void Initialize();

    // This is like a shortcut,
    // Turns ON all the non-NULL MIDI devices.
    void refreshMIDIDevices();

    // Does not care about the devices presently listening to ,
    // Turns on all the devices which are registered as MIDI inputs
    void listenFromAllMIDIInputs();


    // Handling the midi from selected midi devices.
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

    /* Sets the mode */
    void setMode(Mode mode);


    /////##################################
    /// DATA STRUCTURES
    /////##################################

    /*
        Nodes will be represented in the form of a set,
        When there is a connection to the output node and an internal connection changed,
        We call the function `BuildTreeAndMakeQueue` on the nodes, which will :
            - Create Extra buffers for multiple outputs(for in-place processing).
            - Create a priority queue with priorities depending on the dependencies.
            - Process the audio based on the priority queue and Synthesize the audio.
    */
    InputMasterGraphNode* InputNode;
    OutputMasterGraphNode* OutputNode;
    std::set<GraphNode*> AllNodes;

    // Called from GraphPage when a new node is added.
    void nodeAdded(GraphNode* newNode) {
        if (!newNode) return;
        AllNodes.insert(newNode);
    }

    // Called when there is a new connection.
    void ConfigurationChanged();

    /*
     * Takes in the set of all nodes :
     *      sets their audio buffers.
     *      pushes them to the priority queue.
     *
     *      Returns true if the tree is valid and the queue is built.
     */
    bool BuildTreeAndMakeQueue();

    /*
     * Called when we completed building the tree and made the queue.
     */
    void SynthesizeAudioForConfig();

    /////##################################

    ///////////////////////
    /////////////////////
    ///////////////////

    // Will be created and placed when the user enters the dimensions and presses `create Instrument canvas`.
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

        int x, y;

        juce::Component* instrumentCanvas;

        std::unique_ptr<juce::TextEditor> size_x;
        std::unique_ptr<juce::TextEditor> size_y;
        std::unique_ptr<juce::TextButton> createCanvasButton;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditPage)
    };

    // Will contain the paint methods for the page, the data what to show will be in the parent(in hierarchy) class.
    class GraphPage : public juce::Component {
    public:
        GraphPage();
        ~GraphPage() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& event) override;

        // Will be used as the callback function from the `showMenuAsync` method for the popup menu.
        static void AddNodeCallback(int result, GraphPage* graphPageComponent);

    private:
        int mid_x, mid_y; // for panning the graph.
        int zoomValue = 0;

        std::unique_ptr<juce::PopupMenu> AddNodesPopupMenu;
        // This stores the submenus in an owned array to make the memory management easy.
        juce::OwnedArray<juce::PopupMenu> subMenuArray;
        std::unique_ptr<juce::LookAndFeel_V4> styles;
        juce::Point<int> lastMouseDownPosition;

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
        juce::Component* instrumentCanvas;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPage)
    };


    class AudioMIDISettingClass : public juce::DocumentWindow {
    public:
        AudioMIDISettingClass(juce::AudioDeviceManager&);
        ~AudioMIDISettingClass() override {}

        void closeButtonPressed() override {
            Instrument::getInstance()->refreshMIDIDevices();
            setVisible(false); // Hide the window when close button is pressed
        }

    private:
        std::unique_ptr<juce::Component> settingPage;

        std::unique_ptr<juce::LookAndFeel_V4> styles;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMIDISettingClass)
    };


    // Because the pages to be displayed are private, get functions for the pages.
    // Please do not use this before creating the pages, or else everything will crash
    juce::Component* getEditPage() {
        return this->editPage.get();
    }

    juce::Component* getGraphPage() {
        return this->viewport.get();
    }

    juce::Component* getPlayPage() {
        return this->playPage.get();
    }


private:
    bool isCanvasPresent;
    std::unique_ptr<juce::Component> Canvas;

    std::unique_ptr<juce::Component> editPage;
    std::unique_ptr<juce::Component> graphPage;
    std::unique_ptr<juce::Component> playPage;

    int size_width, size_height;
    int tabWidth;

    // The graph node is actually inside this.
    std::unique_ptr<juce::Viewport> viewport;

    Mode presentMode;

    // Decides if we should break the present audio processing, set to true when the configuration changes.
    std::atomic<bool> breakProcessing;

    // The priority queue we are going to use.(impl in MyDataStructures.h)
    PriorityQueue* nodeProcessingQueue;

    juce::OwnedArray<juce::MidiInput> midiInputs;
    std::unique_ptr<juce::DocumentWindow> AudioMIDISettingsJUCE;

    // Device manager and related stuff.
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;

    bool TreeFeasible = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
