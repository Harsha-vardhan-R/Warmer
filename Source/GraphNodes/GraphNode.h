/*
  ==============================================================================

    GraphNode.h
    Created: 22 Feb 2024 4:51:24pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>
#include "../ColourPalette.h"
#include "InputOutputTypesForSokets.h"
#include "Connection.h"

#define PI 3.1415926
#define PI_HALF 1.5707963
#define TAU 6.2831853

class EmptyLookAndFeel : public juce::LookAndFeel_V4 {
public:

    EmptyLookAndFeel() {
        setColour(juce::Slider::ColourIds::backgroundColourId, GraphSliderBackgroundID);
        setColour(juce::Slider::ColourIds::thumbColourId, GraphSliderThumbID);
        setColour(juce::Slider::ColourIds::trackColourId, GraphSliderOutlineID);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, GraphSliderTextID);

        setColour(juce::ComboBox::ColourIds::backgroundColourId, MenuBackgroundID);
        setColour(juce::ComboBox::ColourIds::textColourId, textColourID);
        setColour(juce::ComboBox::ColourIds::arrowColourId, ComboBoxArrowColourID);
    }

    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                     int buttonW, int buttonH, juce::ComboBox& box) override {
        g.setColour(GraphSliderOutlineID);
        g.drawRoundedRectangle(box.getLocalBounds().toFloat() ,3.0f, 2.0);
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(box.getLocalBounds().reduced(2).toFloat(), 3.0f);

        juce::Path arrow;
        float arrowDimen = 5;
        float arrowX = width - 15; // X position of the arrow
        float arrowY = (height - arrowDimen) / 2; // Y position of the arrow

        if (box.isPopupActive()) {
            arrow.addTriangle(arrowX + (arrowDimen * 0.5f) + 1, arrowY,
                              arrowX, arrowY + arrowDimen,
                              arrowX + arrowDimen + 2, arrowY + arrowDimen);
        } else {
            arrow.addTriangle(arrowX, arrowY,
                              arrowX + arrowDimen + 2, arrowY,
                              arrowX + (arrowDimen * 0.5f) + 1, arrowY + arrowDimen);
        }

        g.setColour(ComboBoxArrowColourID);
        g.fillPath(arrow);
    }

    void drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator,
                                           bool isActive,
                                           bool isHighlighted,
                                           bool isTicked,
                                           bool hasSubMenu,
                                           const juce::String& text,
                                           const juce::String& shortcutKeyText,
                                           const juce::Drawable* icon,
                                           const juce::Colour* textColourToUse) override {

        juce::Rectangle<int> highLight(area.getX() + 3, area.getY(), area.getWidth() - 6 , area.getHeight());
        juce::Rectangle<int> aligner(area.getX() + 15, area.getY(), area.getWidth() , area.getHeight());

        auto width = area.getWidth();


        if (isSeparator) {
            g.setColour(SeparatorColourID);
            juce::Rectangle<int> sepLine(area.getX()+3, area.getCentreY(), area.getWidth()-6, 1);
            g.fillRect(sepLine);
            return;
        } else if (isHighlighted) {
            g.setColour(MenuMouseOverColourID);
            g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
            g.setColour(SelectedTabTextColourID);
        } else {
            g.setColour(IdleTabTextColourID);
        }

        // Draw a side arrow if there is a submenu.
        if (hasSubMenu) {
            juce::Path arrow;
            arrow.addTriangle( width - 12,  6,
                               width - 6,  10,
                               width - 12,  14);
            g.setColour(ComboBoxArrowColourID);
            g.fillPath(arrow);
        }

        g.drawText(text, aligner, juce::Justification::centredLeft);

    }

    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override {
        g.setColour(MenuBackgroundID);
        g.fillAll();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyLookAndFeel)
};

class menuStyleJuceNode : public juce::LookAndFeel_V3 {
public:
    menuStyleJuceNode() {}
};

// Class that all the nodes inherit.
class GraphNode : public juce::Component,
                  public juce::AudioProcessor {
public :


    class menuStyleJuce : public juce::LookAndFeel_V3 {
    public:
        menuStyleJuce() {}
    };

    // Socket is a block inside the GraphNode.
    // contrary to the name you can actually have a parameterCtrl without the socket to control,
    // the behaviour will be controlled from the value.
    class Socket : public juce::Component {
    public:

        menuStyleJuce menu_style;

        direction dir;

        // if this is empty then we do not draw a socket input for this socket.
        std::set<SocketDataType> TypesAccepted;

        juce::String name;


        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
        // API calls used from the GraphNode while processing.
        // set and get sets and gets the

        // gets the value from the parameter ctrl,
        // or if connected from the previous socket.
        // not to be called on output sockets.
        float getValue();
        juce::AudioBuffer<float>* getBufferPointer();
        juce::MidiBuffer* getMidiMessage();

        // set value to all the connections this socket is connected to.
        void setFloatValue(float f);
        void setBufferPointer(juce::AudioBuffer<float>* bufferPointer);
        void setMidiMessagePointer(juce::MidiBuffer* midiMessagePointer);


        // isMust must be connected or else the queue will not be built.
        // for example an empty input for audio signal will stop the whole process.
        Socket( juce::String name,
                direction dir,
                bool isMust);

        // Be careful while using this,
        // The program will have undefined behaviour if there is no implementation for this type in the process method.
        void acceptType(SocketDataType type);

        // called after a successful connection , sets the variables and others.
        void connected(Socket* otherPointer, Connection* connection);

        // check if this node can accept this type.
        bool accepts(SocketDataType type) ;

        // If the parameter control is set this will update.
        void update();

        // if the node is on the input side then, this will return
        // a pointer to the node it is from.
        // this should only be called to check on for input sockets.
        // for the output sockets we will get the prev nodes by calling it on the
        // next node.
        void* getPrevNode();

        // returns the point to draw the connection line from or to.
        juce::Point<int> getPivotPos();

        void paint (juce::Graphics& g) override;

        // Add a custom parameter control
        // #### MOST OF THESE FUNCTIONS ARE MEANT TO BE USED ONLY BY dir `IN` Sockets. ####
        //
        void addMenuParameterControl();
        //
        void addSliderParameterControl(float from, float to, float default_);

        // If the parameter control is set to MenuListParameterCtrl, this is used to add new types of menu options.
        // else it is not going to change anything and there is no reason to use this method.
        void addMenuItem(juce::String name);

        // setting the type of output node,
        // this is the type that is checked against the set of input types that can be
        // accepted from an input node.
        void setOutputType(SocketDataType a);

        SocketDataType getOutputType();

        // like getOutputType but this returns the type of the connection presently,
        // meant to be called on an IN, if the socket can accept multiple types
        // you can use this to know what type of input is connected now,
        // would not make any sense to call this on an output node.
        SocketDataType getConnectionType();

        juce::PopupMenu menu;

        // relative to the edit page.
        juce::Point<float> getRelToGlobal(juce::Point<int> p);

        //deleting the socket connection.
        void mouseDown(const juce::MouseEvent& event) override;

        static void deleteSocketCallBack(int result, Socket* thisInstance);

        // called from the socket this is connected to.
        void deletedThisConnectionFrom(Socket* thatSocket);


        // using from two different methods.
        Connection* newConnection = nullptr;
        // adding a new connection.
        void mouseDrag(const juce::MouseEvent& event) override;

        // adding a new connection.
        void mouseUp(const juce::MouseEvent& event) override;

        bool isThisConnected() { return isConnected; }

        void resized() override;

        bool isMust;

        // +++++++++++++++++++++++++++++++
        int repaintConnection();

        // internal API call to  delete the connections from a socket,
        // if called on a input socket delete only the connection,
        // for an output socket we delete all the connections from it.
        void deleteConnections();

        ~Socket() override;

        std::set<Socket*> to;

        bool hasParameterController() {
            return (parameterController.getType() != -1);
        }

        int getTotHeight() {
            if (isConnected) return 20;
            else return 20 + parameterController.getHeight();
        }


        // This is the function that is going to get called from the,
        // Parameter Ctrl if if the method `triggerSocketsTrigger` is called on it.
        // this call then sends the signal to the parent Graph node,
        // where we change callback or something similar based on the changed state.
        static void triggerGraphNodeMiniReset(Socket* thisInstance) {
            std::cout << "Reset triggered from Parameter Ctrl" << "\n";
        }


    private:

        juce::Component* editPage;

        bool isConnected = false;
        Socket* from = nullptr;

        int numberOfSocketsConnectedTo = 0;


        class parameterCtrlLookAndFeel : public juce::LookAndFeel_V3 {
        public:

            parameterCtrlLookAndFeel() {
                setColour(juce::Slider::ColourIds::textBoxTextColourId, GraphSliderTextID);
                setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey);
                setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::white);
                setColour(juce::Slider::ColourIds::thumbColourId, GraphSliderThumbID);


                setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::white);
                setColour(juce::PopupMenu::ColourIds::textColourId, textColourID);
                setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, textSelectedColourID);
            }

        };

        // invisible container that contains other controls.
        class ParameterCtrl : public juce::Component,
                              private juce::Slider::Listener,
                              private juce::ComboBox::Listener {
        public:

            Socket* parentInstance = nullptr;

            ParameterCtrl(Socket* parentInstance) {
                this->parentInstance = parentInstance;
                setVisible(false);
                valueAtomic.store(0.0);
            }

            ~ParameterCtrl() override {};

            // getValue() is the abstraction that is called from process in the respective node to return the
            // selected item or a value.
            //
            // and because floating points can have precision errors better to compare with a greater than or less than
            // rather than equals to if you mean to return a integer.
            float getValue() {
                return valueAtomic.load();
            };

            int getType() {
                return parameterType;
            }

            int getHeight() {
                if (parameterType == -1) return 0;
                else return 30;
            }

            void createNewMenu() {
                if (parameterType != -1) {
                    std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
                    return;
                }

                menuList.reset(new juce::ComboBox);
                menuList.get()->addListener(this);

                menuList.get()->setLookAndFeel(&styles);

                valueAtomic.store(0.0);
                parameterType = 1;
            }

            // call this if you need to call the mini reset on the graph node
            // that this parameterController is in.
            void triggerParent() {
                triggerGraphNodeMiniReset(parentInstance);
            }

            // stores the selected value in a thread safe variable,
            // also sends a message to the GraphNode with the selected ID.
            void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override {
                valueAtomic.store(comboBoxThatHasChanged->getSelectedId());
                triggerParent();
            }

            void addItemToList(juce::String name) {
                if (menuList.get()) {
                    menuList.get()->addItem(name, index);
                    index++;
                }
            }

            void createTextEditor(float from , float to , float val) {
                if (parameterType != -1) {
                    std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
                    return;
                }

                sliderFloat.reset(new juce::Slider());
                sliderFloat.get()->setRange(from, to, 0.001);
                sliderFloat.get()->setValue(val);
                sliderFloat.get()->setSliderStyle(juce::Slider::LinearBar);
                sliderFloat.get()->addListener(this);
                sliderFloat.get()->setNumDecimalPlacesToDisplay(3);

                sliderFloat.get()->setLookAndFeel(&styles);

                this->from = from;
                this->to = to;
                this->value = val;

                parameterType = 2;
            }


            // called from the socket when ready to make it visible.
            void update() {
                // checking if it is not nullptr.
                if (menuList.get()) {
                    menuList.get()->setBounds(getLocalBounds().reduced(4));
                    addAndMakeVisible(menuList.get());
                    // do not ask me if you did not even add one option, dude.
                    menuList.get()->setSelectedId(1, juce::sendNotificationSync);
                } else if (sliderFloat.get()) {
                    sliderFloat.get()->setBounds(getLocalBounds().reduced(4));
                    addAndMakeVisible(sliderFloat.get());
                    sliderFloat.get()->setValue(value);
                }
            }

            void sliderValueChanged(juce::Slider* slider) override {
                float value = sliderFloat.get()->getValue();
                valueAtomic.store(value);
            }

            // lock and unlock while reading the values(presently not used anywhere).
            void lock() { mutex.lock(); }

            void unlock() { mutex.unlock(); }

            void paint(juce::Graphics& g) override {}
            void resized() override {}

            int parameterType = -1; // means nothing.
            // ^ 1 for menu and two for the text input.

            void tellParentsParentToCallMiniReset() {

            }


        private:

            // set from the callbacks and read from get value.
            std::atomic<float> valueAtomic;

            parameterCtrlLookAndFeel styles;


            // This is to lock this when others are reading or setting it.
            // not used anywhere in the code base now
            // because we process nodes one by one we do not set and get on the same socket at the same time.
            std::mutex mutex;

            // THE MENU
            // basically a list of values from which one can be selected.
            // The discrete knob outputs an integer value that can be used to set the selected value.
            std::unique_ptr<juce::ComboBox> menuList;
            int index = 1;


            // THE SLIDER : used by many parameters, look and use inspired by blender's nodes in the graph editor.
            // we will be able to set the value directly, drag the mouse to change the value or use a other node to control this.
            // when we are sliding or setting the value directly in the node, we do not record what the user is doing so
            // the resolution of changing will be of the buffer size, but connecting to an external node or an UI item will
            // give a smooth change in the controls - this is how it is designed to work, NOT a BUG.
            // value is always floating.
            std::unique_ptr<juce::Slider> sliderFloat;
            float from, to, value;


            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterCtrl)
        };

        ParameterCtrl parameterController;

        juce::Point<int> lastMousePosition;

        // Only set for Output nodes.
        SocketDataType type = SocketDataType::NULLType;

        Connection* connectionPointer = nullptr;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
    };

    // Name of the node(shown at the top)
    juce::String name;

    menuStyleJuceNode menu_style;

    /*
     * Returns a vector of dependent nodes.
     */
    std::vector<GraphNode*> getDependencies();

    // returns a vector of values that contains all the nodes this is output to.
    std::set<GraphNode*> getDependents();

    // returns the Nodes that are dependent on the AudioBuffer that is written on by this node,
    // if none exist we are going to return an empty vector.
    std::set<GraphNode*> getAudioBufferDependencies();


    int zoomLevel;
    juce::OwnedArray<Socket> InputSockets;


    // YOU CAN HAVE AT-MOST ONE SOCKET OF THE TYPE SocketDataType::AudioBufferFloat
    juce::OwnedArray<Socket> OutputSockets;

    // The dimensions will be decided by the number of sockets.
    int UIWidth, UIHeight;

    // appears if you want to delete the node.
    juce::PopupMenu menu;


    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y);

    // returns true if all the Input Sockets that are `isMust` are connected.
    virtual bool allGood();

    static void deleteNodeCallback(int result, GraphNode* graphNodeInstance);

    // for moving and delete menu.
    void mouseDown(const juce::MouseEvent& event) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

    // deletes all the connections that come to this node,
    // and are from this node
    void deleteAllConnectionsToAndFromNode();


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g);

    // sets all the sockets in both input and output socket arrays to
    // addAndMakeVisible
    void makeAllSocketsVisible();


    void paint(juce::Graphics& g) override;

    void resized() override;

    ~GraphNode() override;

//    Copy and Implement for your own node.
//
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {}
//    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override {}
//    void releaseResources() override {}
//    void reset() override {}

    double sampleRate;
    int estimatedSamplesPerBlock;

    // Pure virtual functions from the juce::AudioProcessor.
    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) {
        this->sampleRate = sampleRate;
        this->estimatedSamplesPerBlock = estimatedSamplesPerBlock;
    }

    // called while destructing the node,
    // be very careful with shared data from the nodes.
    virtual void releaseResources() = 0;

    // this function is never used , we call the function called `processGraphNode`, to process the node.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {}

    // called after the prepareToPlay is called,
    // use it if you want to configure more things after
    // setting the bufferSize and rate.
    //======
    // this should be used to set the call-back function pointer and the respective data that is,
    // needed for the respective callback function to work.
    //======
    // you basically manage the data while creating and removing a connection.
    // This function will be called every time a connection is added or removed from
    // any og the sockets connected to this
    //=====
    // the audio buffer that needs to get written will get the
    virtual void reset() = 0;

    // mini reset is called when a parameter is changed,
    // the parameterCtrl(if mentioned in the callback), will tell the
    // node processor to stop and call this function here.
    // no mutex locks or processing stops, do only atomic or non processGraphNode related stuff.
    virtual void mini_reset() {}


    // called from the Socket to inform that parameterCtrl has


    juce::AudioBuffer<float>* bufferToWritePointer;
    // set false if this node does not send out an audio buffer as an output.
    bool needAudioBuffer = true;

    // sets the buffer to write, done from the processing queue.
    void setToWriteAudioBuffer(juce::AudioBuffer<float>* b) { bufferToWritePointer = b; }

    // get the set bufferToWritePointer, if not set, returns nullptr
    juce::AudioBuffer<float>* getToWriteAudioBuffer() { return bufferToWritePointer; }


    // the master function that is called from the AudioThread fo each audio buffer,
    // Make sure anything you access from this function is either thread synchronised,
    // or you are 100% sure that it is not going to be accessed from any other thread.
    //
    // The connection has all the stuff you just need, just ask it.
    virtual void processGraphNode() = 0;

    // loading and saving of presets.
    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;



    ///|=========================================|
    // Virtual functions that have their definitions empty.
    const juce::String getName() const override;

    double getTailLengthSeconds() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String& newName) override;

    // +++++++++++++++++++++++

    bool canBeDeleted;


    // this not done by default because only a small
    // percentage of nodes need to be locked.
    // and mostly the use case is to change the callback pointer or something like that.
    std::mutex mutex;








private:

    EmptyLookAndFeel style;

    juce::Point<int> lastMouseDownPosition;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
