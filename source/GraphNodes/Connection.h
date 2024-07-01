/*
  ==============================================================================

    Connection.h
    Created: 8 Apr 2024 11:36:54am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "InputOutputTypesForSokets.h"


#include <juce_graphics/juce_graphics.h>
#include <juce_audio_basics/juce_audio_basics.h>

// connections between sockets.

// This class does not need any kind of thread synchronisation because,
// the get and set functions will(should) never get called at the same time,
// Proof :-
//      set only by the output nodes, get by the input nodes.
//      only one node is processed at a time,
//      a connection is either an IN or an OUT for this node
//      it is not possible that you call a get and a set at the same time,
//      even if it was possible try calling two functions on a same class from a single thread.
//      satisfied ?!




// Owned by GraphPage, they are not considered as components,
// as the bounds will overlap the nodes and everything becomes messy,
// they are just drawn as normal lines in the GraphPage.
class Connection  {
public:

    Connection();

    // this function will be called if there is a successful connection.
    void confirmConnection(void* fromNode, void* fromSocket, void* toNode, void* toSocket);

    // set from the OUT socket, that sets the type of the output,
    // etc...
    // arg1 and 2 do not mean anything right now , but maybe used to set some other things in the future.
    void setConnectionParams(SocketDataType outType, int arg1, int arg2);

    // Should be strictly used after the connection is confirmed,
    // There is no checking here.
    void* getFromNode();
    void* getToNode();

    void* getFromSocket();
    void* getToSocket();

    SocketDataType getType() {
        return connectionType;
    }

    // ==========================
    void* fromSocket, *toSocket;
    void* fromNode, *toNode;

    // this is the way a connection kind of knows what the type of data is flowing through it.
    SocketDataType connectionType = SocketDataType::NULLType;
    int arg1 = -1, arg2 = -1;

    // data flow
    int valueInt = -1;
    float valueFloat = -1.0;
    void* valueVoidPtr = nullptr;



    // set by the OUT node and read by all the input nodes.
    // once every buffer.
    juce::AudioBuffer<float>* bufferPointer = nullptr;
    float valueKept = -1.0;
    juce::MidiBuffer* midiMessagePointer = nullptr;


    void setBufferPointer(juce::AudioBuffer<float>* p) { bufferPointer = p; }
    void setMidiMessagePointer(juce::MidiBuffer* m) { midiMessagePointer = m; }
    void setFloatValue(float f) { valueKept = f; }


    juce::AudioBuffer<float>* getBufferPointer() { return bufferPointer; }
    float getFloatValue() { return valueKept; }
    juce::MidiBuffer* getMidiMessage() { return midiMessagePointer; }

    juce::Colour fromColour = juce::Colours::grey , toColour = juce::Colours::grey;

private:

    juce::Point<float> from;
    juce::Point<float> to;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Connection)
};
