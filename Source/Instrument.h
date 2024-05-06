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
#include <climits>

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


    // function called when setting the volume.
    void setAudioDeviceVolume(float val) {
        nodeProcessingQueue.setVolumeLevel(val);
    }


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

    // returns component at the position in the Graph Page
    Socket* getComponentInGraphPage(juce::Point<float> p) {
        return dynamic_cast<Socket*>(graphPage.get()->getComponentAt(p));
    }

    // Called from GraphPage when a new node is added.
    void nodeAdded(GraphNode* newNode) {
        if (!newNode) return;
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->AllNodes.insert(newNode);
        casted->addAndMakeVisible(newNode);
		nodeProcessingQueue.push(newNode);
    }

    // Called from a GraphNode instance when it is deleted.
    void nodeDeleted(GraphNode* nodePointer) {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        // related connections will be erased while destructing the sockets.
        casted->AllNodes.erase(nodePointer);
		nodeProcessingQueue.remove(nodePointer);
        delete nodePointer;
    }

    // making the connection visible as soon as it is formed,
    // if the connection isn't successful it will be deleted in the socket,
    // child will be automatically removed.
    void connectionInst(Connection* newConnection, juce::Line<int> l) {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->connectionInit(newConnection, l);
    }

    void connectionInitFail() {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->connectionInitFail();
    }

    // calls similar function in graph page to change the connection `Line`.
    void connectionPositionUpdated(Connection* connectionPointer,
                                   juce::Line<int> newLinePosition) {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->updateConnectionLine(connectionPointer, newLinePosition);
    }

    void connectionAdded(Connection* newConnection) {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->connectionAdded(newConnection);
        ConfigurationChanged();
    }

    void connectionRemoved(Connection* connectionObjPointer) {
        GraphPage* casted = (GraphPage*)(graphPage.get());
        casted->connectionRemoved(connectionObjPointer);
        ConfigurationChanged();
    }

    // Called when there is a new connection.
    void ConfigurationChanged();

    /*
     * Sets the new audio parameters like sampleSize and rate for the
     * nodeProcessingQueue.
     */
    bool updateTreeParams();

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

        Connection* temp = nullptr;
        juce::Line<int> p;

        void connectionInit(Connection* n, juce::Line<int> position);

        void connectionInitFail();

        // add new connection, object is created elsewhere,
        // we take ownership in abstraction.
        void connectionAdded(Connection* newConnection);

        // delete this connection and erase it from screen.
        void connectionRemoved(Connection* connectionPointer);

        // updates the position of the connection line, called from socket from graph node
        // when node is dragged.
        void updateConnectionLine(Connection* connectionPointer, juce::Line<int> newLine);

        // draws all the connections.
        void drawConnections(juce::Graphics& g);


        std::set<GraphNode*> AllNodes;
        // keeping track of all the Connections that are present till now..
        std::set<Connection*> AllConnections;
        // stores the Connection* to it's line position.
        // used to draw and update positions.
        std::unordered_map<Connection*, juce::Line<int>> ConnectionToLineMap;

        // dirty region that expands or contracts based on the present lines,
        // because drawing everything is way too expensive.
        juce::Rectangle<int> repaintArea;

        // called when a new node is added or removed, updates the repaintArea rectangle.
        void updateRepaintArea();

        // updates the repaint area based only for one connection
        void updateRepaintArea(Connection* connection);

        // for instances.
        void updateRepaintArea(juce::Line<int>& l);


        // Will be used as the callback function from the `showMenuAsync` method for the popup menu.
        // that shows what nodes can be added.
        static void AddNodeCallback(int result, GraphPage* graphPageComponent);


        juce::Component* getBackground() {
            return componentBackground.get();
        }

    private:

        class BackGroundGraphPageCanvas : public juce::Component {
        public:

            BackGroundGraphPageCanvas();
            ~BackGroundGraphPageCanvas() {}


            void paint(juce::Graphics& g) override ;
            void resized() override;

        private:

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackGroundGraphPageCanvas)
        };

        int mid_x, mid_y; // for panning the graph.
        int zoomValue = 0;

        // this is basically like a background that stays there for the colour,
        // as redrawing the connections on this is expensive(fillAll),
        // we use another transparent component over it that is used to contain,
        // the connections and the nodes.
        std::unique_ptr<juce::Component> componentBackground;

        std::unique_ptr<juce::PopupMenu> AddNodesPopupMenu;
        // This stores the submenus in an owned array to make the memory management easy.
        juce::OwnedArray<juce::PopupMenu> subMenuArray;
        std::unique_ptr<juce::LookAndFeel_V4> styles;
        juce::Point<int> lastMouseDownPosition;

        Instrument* instrumentClassPointer;

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
        ~AudioMIDISettingClass() override {
            settingPage.get()->setLookAndFeel(nullptr);
        }

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

    // AudioProcessingGraph that contains the AudioProcessors i.e the GraphNodes.
    juce::AudioProcessorGraph audioGraph;


    // Decides if we should break the present audio processing, set to true when the configuration changes.
    std::atomic<bool> breakProcessing;

    // The priority queue we are going to use.(impl in MyDataStructures.h)
	// this will always be in the first place of the topo sorted node list.
    PriorityQueue nodeProcessingQueue;

    juce::OwnedArray<juce::MidiInput> midiInputs;
    std::unique_ptr<juce::DocumentWindow> AudioMIDISettingsJUCE;

    // Device manager and related stuff.
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;

    bool TreeFeasible = false;

    // if the instrument class is being destructed.
    bool isBeingDestructed = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
