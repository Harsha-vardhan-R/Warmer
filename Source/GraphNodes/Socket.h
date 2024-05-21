/*
  ==============================================================================

    Socket.h
    Created: 10 Mar 2024 3:35:29pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "InputOutputTypesForSokets.h"
#include "Connection.h"
#include "../ColourPalette.h"

//==============================================================================


// Socket is a block inside the GraphNode.
// contrary to the name you can actually have a parameterCtrl without the socket to control,
// the behaviour will be controlled from the value.
class Socket : public juce::Component {
public:

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

        ParameterCtrl() {
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

        // stores the selected value in a thread safe variable,
        // also sends a message to the GraphNode with the selected ID.
        void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override {
            valueAtomic.store(comboBoxThatHasChanged->getSelectedId());
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



