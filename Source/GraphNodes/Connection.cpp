/*
  ==============================================================================

    Connection.cpp
    Created: 8 Apr 2024 3:42:30pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "Connection.h"

Connection::Connection() {}

void Connection::confirmConnection(void* fromNode, void* fromSocket, void* toNode, void* toSocket) {

    this->fromNode = fromNode;
    this->fromSocket = fromSocket;

    this->toNode = toNode;
    this->toSocket = toSocket;
}

void* Connection::getFromNode() { return fromNode; }
void* Connection::getToNode() { return toNode; }

void *Connection::getFromSocket() { return fromSocket; }
void *Connection::getToSocket() { return toSocket; }