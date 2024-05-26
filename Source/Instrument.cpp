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
#include "Profiler.h"

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

// Please do not mess with the order of statements.
Instrument::Instrument(int tabWidth) {
    Instrument::instancePtr = this;

    this->tabWidth = tabWidth;

    presentMode = Mode::Play;

    InputNode = new InputMasterGraphNode(100, 300);
    OutputNode = new OutputMasterGraphNode(1300, 300);

    nodeProcessingQueue.setInputNode(InputNode);
    nodeProcessingQueue.setOutputNode(OutputNode);

    editPage.reset(new Instrument::EditPage());
    graphPage.reset(new Instrument::GraphPage());
    playPage.reset(new Instrument::PlayPage());

    viewport.reset(new juce::Viewport());
    GraphPage* casted = (GraphPage*)graphPage.get();
    viewport.get()->setViewedComponent(casted->getBackground(), false);
    //viewport.get()->setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::nonHover);


    // Initiating and setting the setup for AudioDeviceSetup.
    deviceManager.reset(new juce::AudioDeviceManager());
    deviceManager.get()->initialise(0, 2, nullptr, true);


    AudioMIDISettingsJUCE = std::make_unique<Instrument::AudioMIDISettingClass>(*deviceManager.get());
    AudioMIDISettingsJUCE.get()->closeButtonPressed();


    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    // nodeProcessingQueue.setInputNode(InputNode);


    // Create and add MidiInput objects to the owned array
    for (const auto& device : midiDevicesHere) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        if (midiInput) {
            midiInput->start(); // start listening to all possible midi in's.
            deviceManager.get()->setMidiInputDeviceEnabled(device.identifier, true);
            midiInputs.add(std::move(midiInput));
        }
    }

    breakProcessing.store(false);

    // adding the node processingQ as a call back.
    deviceManager.get()->addAudioCallback(&nodeProcessingQueue);

    // setting the buffer rate and size for the first time.
    updateTreeParams();

    resized();
    
}

void Instrument::refreshMIDIDevices() {
    // Firstly remove all the devices.
    while (midiInputs.size() != 0) {
        midiInputs.remove(0);
    }

    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    for (const auto& device : midiDevicesHere) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        // Checking if the midi input is actually enabled by the user in AudioMIDIsettings.
        if (midiInput != nullptr && deviceManager.get()->isMidiInputDeviceEnabled(device.identifier)) {
            midiInput->start();
            midiInputs.add(std::move(midiInput));
        }
    }
	
//    std::cout << "Listening From : " << "\n";
//    for (auto i : midiInputs) {
//        std::cout << i->getName() << "\n";
//    }
}

void Instrument::listenFromAllMIDIInputs() {
    // Firstly remove all the devices.
    while (midiInputs.size() != 0) {
        midiInputs.remove(0);
    }


    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    for (const auto& device : midiDevicesHere) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        if (midiInput != nullptr) {
            deviceManager.get()->setMidiInputDeviceEnabled(device.identifier, true); // will put the tick mark.
            midiInput->start();
            midiInputs.add(std::move(midiInput));
        }
    }

//    std::cout << "Listening From : " << "\n";
//    for (auto i : midiInputs) {
//        std::cout << i->getName() << "\n";
//    }

    
}

// TODO : ask to save changes, before replacing.
void Instrument::Initialize() {
    audioGraph.releaseResources();
    // Just replacing all the changed pages to new ones.
    // because of unique pointers, the previous pages will get deleted automatically.
    editPage.reset(new Instrument::EditPage());
    graphPage.reset(new Instrument::GraphPage());
    playPage.reset(new Instrument::PlayPage());

//    for (GraphNode* i : AllNodes) delete i;
}

Instrument::~Instrument() {
    nodeProcessingQueue.processingStop();
    deviceManager.get()->removeAudioCallback(&nodeProcessingQueue);

    isBeingDestructed = true;

    nodeProcessingQueue.flush();

    delete InputNode;


//    delete OutputNode;
}

void Instrument::paint(juce::Graphics& g) {}


void Instrument::setMode(Mode mode) {
    if (mode == Mode::Play) {
        getPlayPage()->addAndMakeVisible(Canvas.get());
    }
    else if (mode == Mode::Edit) {
        getEditPage()->addAndMakeVisible(Canvas.get());
    }
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

    instrumentCanvas = nullptr;

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

    if (instrumentCanvas != nullptr) instrumentCanvas->resized();
}

void Instrument::EditPage::paint(juce::Graphics &g) {
    g.fillAll(EditPageBackColourOneID);
}

void Instrument::EditPage::createCanvasButtonClicked() {

    auto InstrumentInstace = Instrument::getInstance();

    juce::String x_dimen = size_x.get()->getText();
    juce::String y_dimen = size_y.get()->getText();

    int x = x_dimen.getIntValue();
    int y = y_dimen.getIntValue();

    if (x_dimen.isNotEmpty() && y_dimen.isNotEmpty()) {
        if (x != 0 && y != 0) {
            if ((x > 100 && x < 2500) && (y > 100 && y < 2500)) { // Create the canvas.
                this->x = x;
                this->y = y;
                InstrumentInstace->Canvas.reset(new InstrumentCanvas(x, y));
                // Deleting the components and adding the canvas.
                size_x.reset(nullptr);
                size_y.reset(nullptr);
                createCanvasButton.reset(nullptr);

                instrumentCanvas = InstrumentInstace->Canvas.get();
                this->addAndMakeVisible(instrumentCanvas);

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
void Instrument::InstrumentCanvas::paint(juce::Graphics &g) {
    g.fillAll(EditPageBackColourTwoID);
}

//////////////////////////////
/////// GRAPH PAGE ///////////
//////////////////////////////
Instrument::GraphPage::GraphPage() {
    componentBackground.reset(new BackGroundGraphPageCanvas());
    componentBackground.get()->resized();

    // Creating and adding the menus.
    AddNodesPopupMenu.reset(new juce::PopupMenu());

    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());
    subMenuArray.add(new juce::PopupMenu());

    subMenuArray[0]->addItem(101, "Utility");// like the utility from ableton.
    subMenuArray[0]->addItem(102, "Convolution");
    subMenuArray[0]->addItem(103, "Latency"); // constant lag.
    subMenuArray[0]->addItem(104, "ADSR");
    subMenuArray[0]->addItem(105, "LFO");

    subMenuArray[1]->addItem(201, "Oscillator");
    subMenuArray[1]->addItem(202, "Custom Oscillator");
    subMenuArray[1]->addItem(203, "Wave-table Oscillator");
    subMenuArray[1]->addItem(204, "Noise");
    subMenuArray[1]->addItem(205, "Random Oscillator");

    subMenuArray[2]->addItem(301, "Reverb");
    subMenuArray[2]->addItem(302, "Delay");
    subMenuArray[2]->addItem(303, "Distortion");
    subMenuArray[2]->addItem(304, "Saturate");
    subMenuArray[2]->addItem(305, "Equalizer");
    subMenuArray[2]->addItem(306, "Analog-ify");
    subMenuArray[2]->addItem(307, "Amplify");

    subMenuArray[3]->addItem(401, "ButterWorth");
    subMenuArray[3]->addItem(402, "Chebyshev");
    subMenuArray[3]->addItem(403, "Digital");

    subMenuArray[4]->addItem(501, "Mul & Add Transform");
    subMenuArray[4]->addItem(502, "Add Or Sub Signals");
    subMenuArray[4]->addItem(503, "FM");
    subMenuArray[4]->addItem(504, "Mix Signals");
    subMenuArray[4]->addItem(505, "Abs Clamp");
    subMenuArray[4]->addItem(506, "Re-Ramp");
    subMenuArray[4]->addItem(507, "Math Clamp");
    subMenuArray[4]->addItem(508, "Abs");

    subMenuArray[5]->addItem(601, "MIDI");
    subMenuArray[5]->addItem(602, "Arpeggiator");
    subMenuArray[5]->addItem(603, "Polyphony");

    subMenuArray[6]->addItem(701, "Single Signal"); // takes in an audio buffer and sends out only one side of it.

    AddNodesPopupMenu.get()->addSubMenu("General", *subMenuArray[0]);
    AddNodesPopupMenu.get()->addSubMenu("Oscillators", *subMenuArray[1]);
    AddNodesPopupMenu.get()->addSubMenu("Effects", *subMenuArray[2]);
    AddNodesPopupMenu.get()->addSubMenu("Filters", *subMenuArray[3]);
    AddNodesPopupMenu.get()->addSubMenu("Math", *subMenuArray[4]);
    AddNodesPopupMenu.get()->addSubMenu("MIDI", *subMenuArray[5]);
    AddNodesPopupMenu.get()->addSubMenu("Etc", *subMenuArray[6]);


    styles.reset(new MyLookAndFeel());
    AddNodesPopupMenu.get()->setLookAndFeel(styles.get());

    componentBackground.get()->addAndMakeVisible(this);

    instrumentClassPointer = Instrument::getInstance();

    instrumentClassPointer->nodeProcessingQueue.push(static_cast<GraphNode*>(instrumentClassPointer->InputNode));
    instrumentClassPointer->nodeProcessingQueue.push(static_cast<GraphNode*>(instrumentClassPointer->OutputNode));

//    AllNodes.insert((GraphNode*)Instrument::instancePtr->InputNode);
    AllNodes.insert(static_cast<GraphNode*>(instrumentClassPointer->OutputNode));

    // Adding the initial nodes.
    addAndMakeVisible(Instrument::instancePtr->InputNode);
    addAndMakeVisible(Instrument::instancePtr->OutputNode);


    repaintArea.setBounds(0, 0, 0, 0);

    resized();
}

Instrument::GraphPage::~GraphPage() {
    for (GraphNode* i : AllNodes) delete i;
    // connections will be automatically deleted while destructing nodes(sockets).
    // for (Connection* i : AllConnections) delete i;
    AddNodesPopupMenu.get()->setLookAndFeel(nullptr);
}

void Instrument::GraphPage::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        lastMouseDownPosition = event.getPosition();
        AddNodesPopupMenu.get()->showMenuAsync(juce::PopupMenu::Options(),
                                               juce::ModalCallbackFunction::forComponent(AddNodeCallback, this));
    }
}

void Instrument::GraphPage::AddNodeCallback(int result, Instrument::GraphPage *graphPageComponent) {

    GraphNode* temp;

    int pos_x = graphPageComponent->lastMouseDownPosition.getX();
    int pos_y = graphPageComponent->lastMouseDownPosition.getY();

    if (result == 101) {
        temp = new Utility(pos_x, pos_y);
    } else if (result == 102) {
        temp = new Convolution(pos_x, pos_y);
    } else if (result == 103) {
        temp = new Latency(pos_x, pos_y);
    } else if (result == 104) {
        temp = new ADSR(pos_x, pos_y);
    } else if (result == 201) {
        temp = new Oscillator(pos_x, pos_y);
    } else if (result == 204) {
        temp = new Noise(pos_x, pos_y);
    } else if (result == 205) {
        temp = new RandomOscillator(pos_x, pos_y);
    } else if (result == 302) {
        temp = new Delay(pos_x, pos_y);
    } else if (result == 501) {
        temp = new AddAndMul(pos_x, pos_y);
    } else if (result == 502) {
        temp = new AddOrSubtract(pos_x, pos_y);
    } else if (result == 504) {
        temp = new MixSignal(pos_x, pos_y);
    } else if (result == 505) {
        temp = new Clamp(pos_x, pos_y);
    } else if (result == 506) {
        temp = new ReRamp(pos_x, pos_y);
    } else if (result == 507) {
        temp = new MathClamp(pos_x, pos_y);
    } else if (result == 508) {
        temp = new Absolute(pos_x, pos_y);
    } else if (result == 0) {
        /* DO NOTHING */
        return;
    } else {
        std::cout << "Returned option from Graph page menu is not handled in the AddNodeCallBack, Option : " << result
                  << "\n";
        return;
    }

    Instrument::getInstance()->nodeAdded(temp);
    graphPageComponent->addAndMakeVisible(temp);
}

void Instrument::GraphPage::resized() {
    // setting the height from parent(componentBackground(BackGroundGraphPageCanvas) here)
    setBounds(0, 0 , getParentWidth(), getParentHeight());
}

void Instrument::GraphPage::paint(juce::Graphics &g) {
    drawConnections(g);
}

void Instrument::GraphPage::connectionInit(Connection *n, juce::Line<int> p) {

    //Profiler("connection init in graph page");

    temp = n;
    this->p = p;
    //updateRepaintArea(p);
    repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}

void Instrument::GraphPage::connectionInitFail() {
    temp = nullptr;
    //updateRepaintArea();
    repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}

void Instrument::GraphPage::connectionRemoved(Connection *connectionPointer) {
    // TODO : need to find a way to adjust the bounding here.

    GraphNode* f = static_cast<GraphNode*>(connectionPointer->fromNode);
    GraphNode* t = static_cast<GraphNode*>(connectionPointer->toNode);

    if (!instrumentClassPointer->isBeingDestructed) instrumentClassPointer->nodeProcessingQueue.connectionRemoved(f, t);

    AllConnections.erase(connectionPointer);
    ConnectionToLineMap.erase(connectionPointer);


    repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}

void Instrument::GraphPage::updateConnectionLine(Connection *connectionPointer, juce::Line<int> newLine) {
    //Profiler("Update connection line");

    ConnectionToLineMap[connectionPointer] = newLine;
    //updateRepaintArea(newLine);
    //repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}


void Instrument::GraphPage::triggerRepaint() {
    repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}

void Instrument::GraphPage::updateRepaintArea() {

    int minX = 5000;
    int minY = 5000;
    int maxX = 0;
    int maxY = 0;

    for (auto [ connection , line ] : ConnectionToLineMap) {
        minX = std::min(minX, std::min(line.getStartX(), line.getEndX()));
        minY = std::min(minY, std::min(line.getStartY(), line.getEndY()));
        maxX = std::max(maxX, std::max(line.getStartX(), line.getEndX()));
        maxY = std::max(maxY, std::max(line.getStartY(), line.getEndY()));
    }

    repaintArea.setBounds(minX, minY, maxX-minX, maxY-minY);

}

void Instrument::GraphPage::updateRepaintArea(Connection *connection) {

    auto line = ConnectionToLineMap[connection];

    int minX, minY, maxX, maxY;

    minX = std::min(repaintArea.getX(), std::min(line.getStartX(), line.getEndX()));
    minY = std::min(repaintArea.getY(), std::min(line.getStartY(), line.getEndY()));
    maxX = std::max(repaintArea.getX()+repaintArea.getWidth(), std::max(line.getStartX(), line.getEndX()));
    maxY = std::max(repaintArea.getY()+repaintArea.getHeight(), std::max(line.getStartY(), line.getEndY()));

    repaintArea.setBounds(minX, minY, maxX-minX, maxY-minY);

}

void Instrument::GraphPage::updateRepaintArea(juce::Line<int>& line) {

    int minX, minY, maxX, maxY;

    minX = std::min(repaintArea.getX(), std::min(line.getStartX(), line.getEndX()));
    minY = std::min(repaintArea.getY(), std::min(line.getStartY(), line.getEndY()));
    maxX = std::max(repaintArea.getX()+repaintArea.getWidth(), std::max(line.getStartX(), line.getEndX()));
    maxY = std::max(repaintArea.getY()+repaintArea.getHeight(), std::max(line.getStartY(), line.getEndY()));

    repaintArea.setBounds(minX, minY, maxX-minX, maxY-minY);

}

//void Instrument::GraphPage::drawConnections(juce::Graphics &g) {
//    g.setColour(juce::Colours::darkgrey);
//
//    if (temp) g.drawLine(p.toFloat(), 1);
//    // draw all the connections with 2px width.
//    for (auto& pair : ConnectionToLineMap) {
//        juce::Line<int>& line = pair.second;
//        g.drawLine(line.toFloat(), 1);
//    }
//}

void drawBezierCurve(juce::Graphics& g, juce::Point<float> endPoint, juce::Point<float> startPoint)
{
    float controlPointDistance = std::abs(endPoint.x - startPoint.x) / 3.0f;

    juce::Point<float> controlPoint1(startPoint.x + controlPointDistance, startPoint.y);
    juce::Point<float> controlPoint2(endPoint.x - controlPointDistance, endPoint.y);

    // Create the path
    juce::Path path;
    path.startNewSubPath(startPoint);
    path.cubicTo(controlPoint1, controlPoint2, endPoint);


    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void Instrument::GraphPage::drawConnections(juce::Graphics &g) {

    //Profiler("Draw connections in graphPage");

    g.setColour(GraphNodeConnectionColourID);

    if (temp) {
        drawBezierCurve(g, p.getEnd().toFloat(), p.getStart().toFloat());
    }

    for (const auto& pair : ConnectionToLineMap) {
        drawBezierCurve(g, pair.second.getEnd().toFloat(), pair.second.getStart().toFloat());
    }
}

void Instrument::GraphPage::connectionAdded(Connection *newConnection) {
    temp = nullptr;

    AllConnections.insert(newConnection);
    juce::Line<int> temp(0, 0, 0, 0);

    ConnectionToLineMap[newConnection] = temp;
    //updateRepaintArea();

    GraphNode* f = static_cast<GraphNode*>(newConnection->fromNode);
    GraphNode* t = static_cast<GraphNode*>(newConnection->toNode);

	instrumentClassPointer->nodeProcessingQueue.newConnection(f, t);

    repaint(instrumentClassPointer->getPartToRepaintInViewPort());
}

Instrument::GraphPage::BackGroundGraphPageCanvas::BackGroundGraphPageCanvas() {
    resized();
}

void Instrument::GraphPage::BackGroundGraphPageCanvas::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::whitesmoke);
}

void Instrument::GraphPage::BackGroundGraphPageCanvas::resized() {
    setBounds(0, 0, 4000, 5500);// maximum size of the canvas for the nodes.
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

    g.fillAll(PlayBackgroundColourID);

    auto InstrumentInstance = Instrument::getInstance();

    g.setColour(textColourID);

    if (InstrumentInstance->Canvas.get() == nullptr) {
        g.drawText("Please Create an Instrument Canvas for this Instrument in the Edit Page",
                   getLocalBounds(),
                   juce::Justification::centred);
    } else {
        if (InstrumentInstance->Canvas.get() != nullptr) addAndMakeVisible(InstrumentInstance->Canvas.get());
    }
}


Instrument::AudioMIDISettingClass::AudioMIDISettingClass(juce::AudioDeviceManager& deviceManager) : DocumentWindow("Audio/MIDI Settings",
                                                                            juce::Colours::white,
                                                                            DocumentWindow::allButtons) {
    // This is to make sure the input audio is set to none.
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup(setup);
    setup.inputDeviceName = "";
    setup.useDefaultInputChannels = false;
    setup.inputChannels.clear();
    deviceManager.setAudioDeviceSetup(setup, true);


    settingPage.reset(new juce::AudioDeviceSelectorComponent(deviceManager,0, 0, 0,256, true, false, true, false));
    settingPage.get()->setBounds(0, 0, getWidth(), getHeight());

    styles.setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);
    styles.setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::darkgrey);
    styles.setColour(juce::PopupMenu::textColourId, juce::Colours::grey);
    settingPage.get()->setLookAndFeel(&styles);

    setContentOwned(settingPage.get(), true);
}


void Instrument::ConfigurationChanged() {

}


bool Instrument::updateTreeParams() {

    if (deviceManager.get()) {
        double sampleSize;
        double sampleRate;

        sampleSize = deviceManager.get()->getAudioDeviceSetup().bufferSize;
        sampleRate = deviceManager.get()->getAudioDeviceSetup().sampleRate;


        nodeProcessingQueue.setBufferSizeAndRate(sampleRate, sampleSize);

        // reset the midi message collector.
        reset(sampleRate);
    } else {
        std::cout << "No active audio device!" << std::endl;
        return false; // nothing to do.
    }

    return true;
}

