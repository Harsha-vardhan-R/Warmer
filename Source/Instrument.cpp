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

#include <memory>
#include "Profiler.h"

Instrument* Instrument::instancePtr = nullptr;

[[maybe_unused]] void* Instrument::VoidPointerToWheelComponent = nullptr;
[[maybe_unused]] void* Instrument::VoidPointerToPianoComponent = nullptr;


// Please do not mess with the order of statements.
Instrument::Instrument(int tabWidth) {
    Instrument::instancePtr = this;

    this->tabWidth = tabWidth;

    presentMode = Mode::Play;

    InputNode = new InputMasterGraphNode(100, 300);
    OutputNode = new OutputMasterGraphNode(1300, 300);

    nodeProcessingQueue.setInputNode(InputNode);
    nodeProcessingQueue.setOutputNode(OutputNode);

    editPage = std::make_unique<Instrument::EditPage>();
    graphPage = std::make_unique<Instrument::GraphPage>();
    playPage = std::make_unique<Instrument::PlayPage>();

    viewport = std::make_unique<juce::Viewport>();
    auto* casted = (GraphPage*)graphPage.get();
    viewport->setViewedComponent(casted->getBackground(), false);
    viewport->setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::all);

    // Initiating and setting the setup for AudioDeviceSetup.
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    deviceManager->initialise(0, 2, nullptr, true);


    AudioMIDISettingsJUCE = std::make_unique<Instrument::AudioMIDISettingClass>(*deviceManager.get());
    AudioMIDISettingsJUCE->closeButtonPressed();


    // Setting up for the MIDI listening.
    auto midiDevicesHere = juce::MidiInput::getAvailableDevices();

    // nodeProcessingQueue.setInputNode(InputNode);


    // Create and add MidiInput objects to the owned array
    listenFromAllMIDIInputs();

    Piano::Instance_Pointer->setMidiNumToListenTo(1);

    breakProcessing.store(false);

    // adding the node processingQ as a call back.
    deviceManager->addAudioCallback(&nodeProcessingQueue);

    // setting the buffer rate and size for the first time.
    updateTreeParams();

    resized();
    
}

void Instrument::refreshMIDIDevices() {
    // Firstly remove all the devices.
    // Remove all the devices.
    while (midiInputs.size() != 0) midiInputs.remove(0);

    // Setting up for the MIDI listening.
    auto midiInputDevices = juce::MidiInput::getAvailableDevices();
    for (const auto& device : midiInputDevices) {
        auto midiInput = juce::MidiInput::openDevice(device.identifier, this);
        if (midiInput != nullptr && deviceManager->isMidiInputDeviceEnabled(device.identifier)) {
            midiInput->start();
            midiInputs.add(std::move(midiInput));
        }
    }

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
            deviceManager->setMidiInputDeviceEnabled(device.identifier, true); // will put the tick mark.
            midiInput->start();
            midiInputs.add(std::move(midiInput));
        }
    }

    
}

// TODO : ask to save changes, before replacing.
void Instrument::Initialize() {
    audioGraph.releaseResources();
    // Just replacing all the changed pages to new ones.
    // because of unique pointers, the previous pages will get deleted automatically.
    editPage = std::make_unique<Instrument::EditPage>();
    graphPage = std::make_unique<Instrument::GraphPage>();
    playPage = std::make_unique<Instrument::PlayPage>();

//    for (GraphNode* i : AllNodes) delete i;
}

Instrument::~Instrument() {
    nodeProcessingQueue.processingStop();
    deviceManager->removeAudioCallback(&nodeProcessingQueue);

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
    auto* parent = (juce::TabbedComponent*)getParentComponent();

    if (parent != nullptr) setBounds(this->tabWidth,
                                     1,
                                     parent->getWidth() - this->tabWidth - 1,
                                     parent->getHeight() - 2);

    if (editPage->getParentComponent() != nullptr) {editPage->resized();}
    if (graphPage->getParentComponent() != nullptr) {graphPage->resized();}
    if (playPage->getParentComponent() != nullptr) {playPage->resized();}

    if (AudioMIDISettingsJUCE != nullptr) AudioMIDISettingsJUCE->setBounds(0, 0, 300, 300);
}


/////////////////////////////
/////// EDIT PAGE ///////////
/////////////////////////////
Instrument::EditPage::EditPage() {

    size_x = std::make_unique<juce::TextEditor>();
    addAndMakeVisible(size_x.get());

    size_y = std::make_unique<juce::TextEditor>();
    addAndMakeVisible(size_y.get());

    createCanvasButton = std::make_unique<juce::TextButton>();
    createCanvasButton->setButtonText("Create Instrument Canvas");
    addAndMakeVisible(createCanvasButton.get());
    createCanvasButton->onClick = [this] { createCanvasButtonClicked(); };

    instrumentCanvas = nullptr;

    resized();
}

void Instrument::EditPage::resized() {

    auto* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30, 1, parent->getWidth() - 29, parent->getHeight() - 2);


    auto bounds = getLocalBounds();
    auto horizontalCenter = bounds.getX()+getHeight()/2;
    auto verticalCenter = bounds.getX()+getWidth()/2;

    if (size_x != nullptr) size_x->setBounds(verticalCenter-180, horizontalCenter, 50, 25);
    if (size_y != nullptr) size_y->setBounds(verticalCenter-125, horizontalCenter, 50, 25);
    if (createCanvasButton != nullptr) createCanvasButton->setBounds(verticalCenter-65, horizontalCenter, 200, 25);

    if (instrumentCanvas != nullptr) instrumentCanvas->resized();
}

void Instrument::EditPage::paint(juce::Graphics &g) {
    g.fillAll(EditPageBackColourOneID);
}

void Instrument::EditPage::createCanvasButtonClicked() {

    auto InstrumentInstance = Instrument::getInstance();

    juce::String x_dimen = size_x->getText();
    juce::String y_dimen = size_y->getText();

    int x = x_dimen.getIntValue();
    int y = y_dimen.getIntValue();

    if (x_dimen.isNotEmpty() && y_dimen.isNotEmpty()) {
        if (x != 0 && y != 0) {
            if ((x > 100 && x < 2500) && (y > 100 && y < 2500)) { // Create the canvas.
                this->x = x;
                this->y = y;
                InstrumentInstance->Canvas.reset(new InstrumentCanvas(x, y));
                // Deleting the components and adding the canvas.
                size_x.reset(nullptr);
                size_y.reset(nullptr);
                createCanvasButton.reset(nullptr);

                instrumentCanvas = InstrumentInstance->Canvas.get();
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
    componentBackground = std::make_unique<BackGroundGraphPageCanvas>();
    componentBackground->resized();

    // Creating and adding the menus.
    AddNodesPopupMenu = std::make_unique<juce::PopupMenu>();

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
    subMenuArray[0]->addItem(105, "Envelope");
    subMenuArray[0]->addItem(106, "Feedback");

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
    subMenuArray[3]->addItem(403, "Non-Resonant Filter");
    subMenuArray[3]->addItem(404, "Resonating Filter");

    subMenuArray[4]->addItem(501, "Mul & Add Transform");
    subMenuArray[4]->addItem(502, "Add Or Sub Signals");
    subMenuArray[4]->addItem(503, "FM Operator");
    subMenuArray[4]->addItem(504, "Cross Fade");
    subMenuArray[4]->addItem(505, "Abs Clamp");
    subMenuArray[4]->addItem(506, "Re-Ramp");
    subMenuArray[4]->addItem(507, "Math Clamp");
    subMenuArray[4]->addItem(508, "Abs");
    subMenuArray[4]->addItem(509, "Mixer");

    subMenuArray[5]->addItem(601, "Invert Events");
    subMenuArray[5]->addItem(602, "Arpeggiator");
    subMenuArray[5]->addItem(603, "Polyphony");
    subMenuArray[5]->addItem(604, "Transpose");
    subMenuArray[5]->addItem(605, "Note Velocity");

    subMenuArray[6]->addItem(701, "Constant");
    subMenuArray[6]->addItem(702, "BPM TO MS");

    AddNodesPopupMenu->addSubMenu("General", *subMenuArray[0]);
    AddNodesPopupMenu->addSubMenu("Oscillators", *subMenuArray[1]);
    AddNodesPopupMenu->addSubMenu("Effects", *subMenuArray[2]);
    AddNodesPopupMenu->addSubMenu("Filters", *subMenuArray[3]);
    AddNodesPopupMenu->addSubMenu("Math", *subMenuArray[4]);
    AddNodesPopupMenu->addSubMenu("MIDI", *subMenuArray[5]);
    AddNodesPopupMenu->addSubMenu("Etc", *subMenuArray[6]);


    styles = std::make_unique<MyLookAndFeel>();
    AddNodesPopupMenu->setLookAndFeel(styles.get());

    componentBackground->addAndMakeVisible(this);

    instrumentClassPointer = Instrument::getInstance();

    instrumentClassPointer->nodeProcessingQueue.push(static_cast<GraphNode*>(Instrument::getInstance()->InputNode));
    instrumentClassPointer->nodeProcessingQueue.push(static_cast<GraphNode*>(Instrument::getInstance()->OutputNode));

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
    AddNodesPopupMenu->setLookAndFeel(nullptr);
}

void Instrument::GraphPage::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        lastMouseDownPosition = event.getPosition();
        AddNodesPopupMenu->showMenuAsync(juce::PopupMenu::Options(),
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
    } else if (result == 105) {
        temp = new Envelope(pos_x, pos_y);
    } else if (result == 106) {
        temp = new Feedback(pos_x, pos_y);
    } else if (result == 201) {
        temp = new Oscillator(pos_x, pos_y);
    } else if (result == 204) {
        temp = new Noise(pos_x, pos_y);
    } else if (result == 205) {
        temp = new RandomOscillator(pos_x, pos_y);
    } else if (result == 302) {
        temp = new Delay(pos_x, pos_y);
    } else if (result == 303) {
        temp = new Distort(pos_x, pos_y);
    } else if (result == 403) {
        temp = new DigitalFilter(pos_x, pos_y);
    } else if (result == 501) {
        temp = new AddAndMul(pos_x, pos_y);
    } else if (result == 502) {
        temp = new AddOrSubtract(pos_x, pos_y);
    } else if (result == 503) {
        temp = new FM(pos_x, pos_y);
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
    } else if (result == 509) {
        temp = new Mixer(pos_x, pos_y);
    } else if (result == 601) {
        temp = new Invert(pos_x, pos_y);
    } else if (result == 603) {
        temp = new Polyphony(pos_x, pos_y);
    } else if (result == 604) {
        temp = new Transpose(pos_x, pos_y);
    } else if (result == 605) {
        temp = new Velocity(pos_x, pos_y);
    } else if (result == 702) {
        temp = new BpmDivToMs(pos_x, pos_y);
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

    auto* f = static_cast<GraphNode*>(connectionPointer->fromNode);
    auto* t = static_cast<GraphNode*>(connectionPointer->toNode);

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

void drawBezierCurve(juce::Graphics& g, juce::Point<float> endPoint, juce::Point<float> startPoint, juce::Colour startGrad, juce::Colour endGrad) {
    float controlPointDistance = std::abs(endPoint.x - startPoint.x) / 3.0f;
    juce::Point<float> controlPoint1(startPoint.x + controlPointDistance, startPoint.y);
    juce::Point<float> controlPoint2(endPoint.x - controlPointDistance, endPoint.y);

    juce::Path path;
    path.startNewSubPath(startPoint);
    path.cubicTo(controlPoint1, controlPoint2, endPoint);

    juce::ColourGradient gradient(startGrad, startPoint, endGrad, endPoint, false);
    g.setGradientFill(gradient);
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void Instrument::GraphPage::drawConnections(juce::Graphics &g) {

    g.setColour(GraphNodeConnectionColourID);

    if (temp) {
        drawBezierCurve(g, p.getEnd().toFloat(), p.getStart().toFloat(), temp->fromColour, temp->toColour);
    }

    for (const auto& pair : ConnectionToLineMap) {
        drawBezierCurve(g, pair.second.getEnd().toFloat(), pair.second.getStart().toFloat(), pair.first->fromColour, pair.first->toColour);
    }
}

void Instrument::GraphPage::connectionAdded(Connection *newConnection) {
    temp = nullptr;

    AllConnections.insert(newConnection);
    juce::Line<int> temp(0, 0, 0, 0);

    ConnectionToLineMap[newConnection] = temp;
    //updateRepaintArea();

    auto* f = static_cast<GraphNode*>(newConnection->fromNode);
    auto* t = static_cast<GraphNode*>(newConnection->toNode);

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
    auto* parent = (juce::TabbedComponent*)getParentComponent();
    if (parent != nullptr) setBounds(30,
              1,
              parent->getWidth() - 29,
              parent->getHeight() - 2);
}

void Instrument::PlayPage::paint(juce::Graphics &g) {

    g.fillAll(PlayBackgroundColourID);

    auto InstrumentInstance = Instrument::getInstance();

    g.setColour(textColourID);

    if (InstrumentInstance->Canvas == nullptr) {
        g.drawText("Please Create an Instrument Canvas for this Instrument in the Edit Page",
                   getLocalBounds(),
                   juce::Justification::centred);
    } else {
        if (InstrumentInstance->Canvas != nullptr) addAndMakeVisible(InstrumentInstance->Canvas.get());
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
//    setResizable(true, false);

    settingPage = std::make_unique<juce::AudioDeviceSelectorComponent>(deviceManager,0, 0, 2,256, true, false, true, false);
    settingPage->setBounds(0, 0, getWidth(), getHeight());

    styles.setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);
    styles.setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::darkgrey);
    styles.setColour(juce::PopupMenu::textColourId, juce::Colours::grey);
    settingPage->setLookAndFeel(&styles);

    setContentOwned(settingPage.get(), true);

    setSize(450, 1200);

}



void Instrument::ConfigurationChanged() {
	// Do nothing for now.
}


bool Instrument::updateTreeParams() {

    if (deviceManager) {
        double sampleSize;
        double sampleRate;

        sampleSize = deviceManager->getAudioDeviceSetup().bufferSize;
        sampleRate = deviceManager->getAudioDeviceSetup().sampleRate;


        nodeProcessingQueue.setBufferSizeAndRate(sampleRate, (int)sampleSize);

        // reset the midi message collector.
        reset(sampleRate);
    } else {
        std::cout << "No active audio device!" << std::endl;
        return false; // nothing to do.
    }

    return true;
}

