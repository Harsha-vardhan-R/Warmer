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
#include "ParameterCtrl.h"


//==============================================================================
/*
*/
class Socket  : public juce::Component {
public:

    direction dir;

    // if this is empty then we do not draw a socket input for this socket.
    std::set<SocketDataType> TypesAccepted;

    juce::String name;

    // isMust must be connected or else the queue will not be built.
    // for example an empty input for audio signal will stop the whole process.
    Socket( juce::String name,
            direction dir,
            bool isMust) {

        this->name = name;
        this->dir = dir;
        this->isMust = isMust;
    }

    // Be careful while using this,
    // The program will have undefined behaviour if there is no implementation for this type in the process method.
    void acceptType(SocketDataType type) { TypesAccepted.insert(type); }

    bool noConnectionSocket() {
        return (TypesAccepted.size() == 0);
    }

    // If the parameter control is set this will update.
    void update() {
        parameterController.setBounds(0, 15, getWidth(), 35);
        parameterController.update();
        addAndMakeVisible(&parameterController);
    }

    // if the node is on the input side then, this will return
    // a pointer to the node it is from.
    // this should only be called to check on for input sockets.
    // for the output sockets we will get the prev nodes by calling it on the
    // next node.
    void* getPrevNode() {

        switch (dir) {
            case (IN):
                return static_cast<void*>(from);
        }

        return nullptr;
    }

    void paint (juce::Graphics& g) override {
        // Small square
        g.setColour(juce::Colours::black);
        if (dir == direction::IN) {
            g.fillRect(0, 5, 5, 5);
            juce::Rectangle<int> bound(10, 0, getWidth()-10, 15);
            g.drawText(name, bound, juce::Justification::centredLeft);
        } else {
            g.fillRect(getWidth()-5, 5, 5, 5);
            juce::Rectangle<int> bound(0, 0, getWidth()-10, 15);
            g.drawText(name, bound, juce::Justification::centredRight);
        }
    }

    // Add a custom parameter control
    // #### MOST OF THESE FUNCTIONS ARE MEANT TO BE USED ONLY BY dir `IN` Sockets. ####
    //
    void addMenuParameterControl() {
        parameterController.createNewMenu();
    }
    //
    void addSliderParameterControl(float from, float to, float default_) {
        parameterController.createTextEditor(from, to, default_);
    }

    // If the parameter control is set to MenuListParameterCtrl, this is used to add new types of menu options.
    // else it is not going to change anything and there is no reason to use this method.
    void addMenuItem(juce::String name) {
        parameterController.addItemToList(name);
    }

    // setting the type of an output node,
    // this is the type that is checked against the set of input types that can be
    // accepted from an input node.
    void setOutputType(SocketDataType a) {
        if (dir == direction::IN) {
            std::cout << "Trying to set the Output type of a Socket that has In direction, but why??" << "\n";
            return;
        }

        type = a;
    }

    //deleting the socket connection.
    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isRightButtonDown()) { // check if the connection is present.
            // Show context menu
//            juce::PopupMenu menu;
//            menu.addItem(1, "Delete Connection");
            //menu.showMenuAsync(juce::PopupMenu::Options(),
            //juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
        } else {
            lastMousePosition = event.getPosition();
        }
    }

    // adding a new connection.
    void mouseDrag(const juce::MouseEvent& event) override {
        if (dir == direction::IN) return;

//        drawLine((float)getWidth()-2.0, 8.0, (float)event.getX(), (float)event.getY());

    }

    // adding a new connection.
    void mouseUp(const juce::MouseEvent& event) override {
        if (dir == direction::IN) return;


    }

    bool isThisConnected() { return isConnected; }

    void resized() override {}

    bool isMust;

private:

    // Helper function that draws connections.
    // soc_one will be the from.
    void makeConnection(void* soc_two) {
        Socket* casted_socket = static_cast<Socket*>(soc_two);

        // if the casting was successful.
        if (casted_socket) {
            if (casted_socket->dir == direction::OUT) return;
            // TODO: check for loops

        }
    }


    bool isConnected = false;
    void* from = nullptr, *to = nullptr;

    ParameterCtrl parameterController;

    juce::Point<int> lastMousePosition;

    // Only set for Output nodes.
    SocketDataType type = NULLType;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
};




