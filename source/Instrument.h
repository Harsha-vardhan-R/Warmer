/*
  ==============================================================================

    Instrument.h
    Created: 15 Feb 2024 6:47:17pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <atomic>
#include <climits>

// Needed juce stuff.
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Modes.h"
#include "Piano.h"
#include "GraphNodes/Collection.h"
#include "MyDataStructures.h"
#include "XML_Handling.h"
#include "juce_core/juce_core.h"



/*
    Contains all the functions related to Instrument.
    Everything from loading an instrument, saving it, playing it and painting it on the component.
*/
class Instrument : public juce::Component,
                   public makeParseXML {
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
    [[maybe_unused]] static void* VoidPointerToPianoComponent;
    [[maybe_unused]] static void* VoidPointerToWheelComponent;

    explicit Instrument(int);
    ~Instrument() override;
    //++++++++++++++++++
    void paint(juce::Graphics& g) override;
    void resized() override;
    //====== INSTRUMENT'S API's ========
    void OpenAudioAndMIDISettings() {
        // AudioMIDISettingsJUCE->setCentrePosition(getParentWidth()/2, getParentHeight()/2);
        // AudioMIDISettingsJUCE->setVisible(true);
    }

    void Initialize();

    // This is like a shortcut,
    // Turns ON all the non-NULL MIDI devices.
    void refreshMIDIDevices();

    // Does not care about the devices presently listening to ,
    // Turns on all the devices which are registered as MIDI inputs
    void listenFromAllMIDIInputs();

    /* Sets the mode */
    void setMode(Mode mode);


    // function called when setting the volume.
    void setAudioDeviceVolume(float val) {
        nodeProcessingQueue.setVolumeLevel(val);
    }

    // top of the recursive calls that
    void createRootTag();

    // Keeps track of present opened file,
    // so if a new file is opened we remember it.
    // unless this is null or not the user presses 
    // `Save as`, anytime asked for save it saves to this file.
    // Loading a new Instrument is going to replace that path to this.
    juce::File presentWorkingfile();
    // This basically tries to create a directory named
    // `warmer`, in the document folder, if this failed
    // Then saving and loading files will open the
    // directory where the executable is present.
    bool tryToCreateWarmerDirInDocuments();

    juce::XmlElement* makeXML() override;
    void parseXMLChildren(juce::XmlElement* x) override;


    // +++++++++++++++++++++++++++++++++++++++++++++++
    // ========== API FROM PluginProcess =============
    // +++++++++++++++++++++++++++++++++++++++++++++++
    // Main method that is going to start the processing.
    void midiInAndProcessAudioInplace(juce::MidiBuffer& midiBuffer, juce::AudioBuffer<float>& buffer);
    void prepareNodeProcessingQueue(double sampleRate, int samplesPerBlock);

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
    GraphNode::Socket* getComponentInGraphPage(juce::Point<float> p) {
        return dynamic_cast<GraphNode::Socket*>(graphPage.get()->getComponentAt(p));
    }

    // Called from GraphPage when a new node is added.
    void nodeAdded(GraphNode* newNode) {
        if (!newNode) return;
        auto* casted = (GraphPage*)(graphPage.get());
        casted->AllNodes.insert(newNode);
//        casted->addAndMakeVisible(newNode);
		nodeProcessingQueue.push(newNode);
    }

    // Called from a GraphNode instance when it is deleted.
    void nodeDeleted(GraphNode* nodePointer) {
        auto* casted = (GraphPage*)(graphPage.get());
        // related connections will be erased while destructing the sockets.
        casted->AllNodes.erase(nodePointer);
		nodeProcessingQueue.remove(nodePointer);
        delete nodePointer;
    }

    // making the connection visible as soon as it is formed,
    // if the connection isn't successful it will be deleted in the socket,
    // child will be automatically removed.
    void connectionInst(Connection* newConnection, juce::Line<int> l) {
        auto* casted = (GraphPage*)(graphPage.get());
        casted->connectionInit(newConnection, l);
    }

    void connectionInitFail() {
        auto* casted = (GraphPage*)(graphPage.get());
        casted->connectionInitFail();
    }

    // calls similar function in graph page to change the connection `Line`.
    void connectionPositionUpdated(Connection* connectionPointer,
                                   juce::Line<int> newLinePosition) {
        auto* casted = (GraphPage*)(graphPage.get());
        casted->updateConnectionLine(connectionPointer, newLinePosition);
    }

    void triggerGraphPageRepaint() {
        auto* casted = (GraphPage*)(graphPage.get());
        casted->triggerRepaint();
    }

    void connectionAdded(Connection* newConnection, bool flag = true) {
        auto* casted = (GraphPage*)(graphPage.get());
        casted->connectionAdded(newConnection, flag);
        ConfigurationChanged();
    }

    void connectionRemoved(Connection* connectionObjPointer) {
        auto* casted = (GraphPage*)(graphPage.get());
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


    // keyboard state that the Piano uses.
    juce::MidiKeyboardState keyboardState;

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
    class EditPage : public juce::Component,
                     public makeParseXML  {
    public:

        EditPage();
        ~EditPage() override {}

        void paint(juce::Graphics& g) override;
        void resized() override;

        juce::XmlElement* makeXML() override;
        void parseXMLChildren(juce::XmlElement* x) override;

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
    class GraphPage : public juce::Component,
                      public makeParseXML  {
    public:
        GraphPage();
        ~GraphPage() override;

        juce::XmlElement* makeXML() override;
        void parseXMLChildren(juce::XmlElement* x) override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& event) override;

        Connection* temp = nullptr;
        juce::Line<int> p;

        void connectionInit(Connection* n, juce::Line<int> position);

        void connectionInitFail();

        void triggerRepaint();

        // add new connection, object is created elsewhere,
        // we take ownership in abstraction.
        void connectionAdded(Connection* newConnection, bool flag);

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

        bool removeConnectionsCall = false;

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
    class PlayPage : public juce::Component,
                     public makeParseXML  {
    public:
        PlayPage();
        ~PlayPage() override {}

        juce::XmlElement* makeXML() override;
        void parseXMLChildren(juce::XmlElement* x) override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        juce::Component* instrumentCanvas;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPage)
    };


    class AudioMIDISettingClass : public juce::DocumentWindow {
    public:
        explicit AudioMIDISettingClass(juce::AudioDeviceManager&);

        ~AudioMIDISettingClass() override {
            settingPage->setLookAndFeel(nullptr);
        }

        void closeButtonPressed() override {
            // try to listen from any new MIDI inputs that are selected.
            Instrument::getInstance()->refreshMIDIDevices();
            Instrument::getInstance()->updateTreeParams();
            getInstance()->updateTreeParams();
            setVisible(false); // Hide the window when close button is pressed
        }

    private:
        std::unique_ptr<juce::Component> settingPage;

        juce::LookAndFeel_V2 styles;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMIDISettingClass)
    };


    // Because the pages to be displayed are private, get functions for the pages.
    // do not use this before creating the pages.
    juce::Component* getEditPage() {
        return this->editPage.get();
    }

    // returns the viewport component that graph page is in.
    juce::Component* getGraphPage() {
        return this->viewport;
    }

    juce::Component* getPlayPage() {
        return this->playPage.get();
    }


    // returns the part of graph page that is visible in the viewport.
    juce::Rectangle<int> getPartToRepaintInViewPort() {
        if (viewport) return viewport->getViewArea();
        return juce::Rectangle(0, 0, 100, 100);
    }

private:
    bool isCanvasPresent{};
    std::unique_ptr<juce::Component> Canvas;

    // do not try to make these class members,
    // they are kind of a little interconnected with  constructors
    // making them class members will initialise before the constructor.
    // if you want to do it please keep in mind it at least will waste a day of your life.
    // with build getting crashed, even gdb does not know where the errors come from.
    std::unique_ptr<juce::Component> editPage;
    std::unique_ptr<juce::Component> graphPage;
    std::unique_ptr<juce::Component> playPage;

    int size_width{}, size_height{};
    int tabWidth;

    // The graph node is actually inside this.
    // std::unique_ptr<juce::Viewport> viewport;
    juce::Viewport* viewport;

    Mode presentMode;

    // AudioProcessingGraph that contains the AudioProcessors i.e the GraphNodes.
    juce::AudioProcessorGraph audioGraph;


    // Decides if we should break the present audio processing, set to true when the configuration changes.
    std::atomic<bool> breakProcessing{};

    // impl in 'MyDataStructures.h'
    TopoSorter nodeProcessingQueue;

    juce::OwnedArray<juce::MidiInput> midiInputs;
    // std::unique_ptr<juce::DocumentWindow> AudioMIDISettingsJUCE;

    // Device manager and related stuff.
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;

    bool TreeFeasible = false;

    // if the instrument class is being destructed.
    bool isBeingDestructed = false;

    std::unique_ptr<juce::FileChooser> _chooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
