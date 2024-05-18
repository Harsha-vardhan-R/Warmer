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
#include "Connection.h"

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
    void repaintConnection();

    // internal API call to  delete the connections from a socket,
    // if called on a input socket delete only the connection,
    // for an output socket we delete all the connections from it.
    void deleteConnections();

    ~Socket() override;

    std::set<Socket*> to;

    bool hasParameterController() {
        return (parameterController.getType() != -1);
    }


private:

    juce::Component* editPage;

    bool isConnected = false;
    Socket* from = nullptr;

    int numberOfSocketsConnectedTo = 0;

    ParameterCtrl parameterController;

    juce::Point<int> lastMousePosition;

    // Only set for Output nodes.
    SocketDataType type = SocketDataType::NULLType;

    Connection* connectionPointer = nullptr;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
};



