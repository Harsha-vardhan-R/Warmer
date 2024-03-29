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

        parameterController.reset(nullptr);

        setBounds(0, 0, 5, 5);
    }

    // Be careful while using this,
    // The program will have undefined behaviour if there is no implementation for this type in the process method.
    void acceptType(SocketDataType type) { TypesAccepted.insert(type); }

    bool noConnectionSocket() {
        return (TypesAccepted.size() == 0);
    }

    // called from resize in Graph node.
    void setParameterCtrlBounds(juce::Rectangle<int> bound) {
        if (parameterController.get() == nullptr) return;
        parameterController.get()->setBoundForCtlr(bound);
    }

    ~Socket() override {}

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
        g.fillAll(juce::Colours::red);
    }

    //deleting the socket connection.
    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isRightButtonDown()) { // check if the connection is present.
            // Show context menu
//            juce::PopupMenu menu;
//            menu.addItem(1, "Delete Connection");
            //menu.showMenuAsync(juce::PopupMenu::Options(),
            //juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
        }
    }

    // Add a custom parameter control
    // #### MOST OF THESE FUNCTIONS ARE MEANT TO BE USED ONLY BY dir `IN` Sockets. ####
    //
    void addMenuParameterControl() {
        if (parameterController.get() != nullptr) {
            std::cout << "Resetting parameter control type is not allowed" << "\n";
            return;
        }

        parameterController.reset(new MenuListParameterCtrl());
    }
    //
    void addSliderParameterControl(float from, float to, float default_) {
        if (parameterController.get() != nullptr) {
            std::cout << "Resetting parameter control type is not allowed" << "\n";
            return;
        }

        parameterController.reset(new NumericSliderParameterCtrl(from, to, default_));
    }

    // If the parameter control is set to MenuListParameterCtrl, this is used to add new types of menu options.
    // else it is not going to change anything and there is no reason to use this method.
    void addMenuItem(juce::String name) {

        MenuListParameterCtrl* control = dynamic_cast<MenuListParameterCtrl*>(parameterController.get());

        if (control == nullptr) {
            std::cout << "The parameter control is not a menu type, return" << "\n";
            return;
        }

        control->addItemToList(name);
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

    // adding a new connection.
    void mouseDrag() {

    }

    bool isThisConnected() { return isConnected; }

    void resized() override {}

    bool isMust;

private:
    bool isConnected = false;
    void* from = nullptr, *to = nullptr;

    std::unique_ptr<ParameterCtrl> parameterController;

    // Only set for Output nodes.
    SocketDataType type = NULLType;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
};
