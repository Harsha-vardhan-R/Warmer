/*
  ==============================================================================

    Connection.h
    Created: 8 Apr 2024 11:36:54am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// connections between sockets.

// Owned by GraphPage, they are not considered as components,
// as the bounds will overlap the nodes and everything becomes messy,
// they are just drawn as normal lines in the GraphPage.
class Connection  {
public:

    Connection();

    // this function will be called if there is a successful connection.
    void confirmConnection(void* fromNode, void* fromSocket, void* toNode, void* toSocket);

    // Should be strictly used after the connection is confirmed,
    // There is no checking here.
    void* getFromNode();
    void* getToNode();

    void* getFromSocket();
    void* getToSocket();

    void* fromSocket, *toSocket;
    void* fromNode, *toNode;

private:

    juce::Point<float> from;
    juce::Point<float> to;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Connection)
};