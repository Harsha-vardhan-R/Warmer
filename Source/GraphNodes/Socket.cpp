/*
  ==============================================================================

    Socket.cpp
    Created: 8 Apr 2024 11:36:38am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Socket.h"

//class Instrument;
#include "../Instrument.h"

Socket::Socket(juce::String name, direction dir, bool isMust) {
    this->name = name;
    this->dir = dir;
    this->isMust = isMust;

    menu.addItem(1, "Remove Connection");

}

void Socket::paint(juce::Graphics &g)  {
    // Small square
    g.setColour(juce::Colours::darkgrey);

    // one for input socket and other for output socket check.
    if (dir == direction::IN) {
        if (TypesAccepted.size()) g.fillRect(0, 5, 5, 5);
        juce::Rectangle<int> bound(10, 0, getWidth()-10, 15);
        if (isMust) g.setColour(juce::Colours::black);
        g.drawText(name, bound, juce::Justification::centredLeft);
    } else {
        if (type != SocketDataType::NULLType) g.fillRect(getWidth()-5, 5, 5, 5);
        juce::Rectangle<int> bound(0, 0, getWidth()-10, 15);
        if (isMust) g.setColour(juce::Colours::black);
        g.drawText(name, bound, juce::Justification::centredRight);
    }

}

juce::Point<int> Socket::getPivotPos() {
    // position of the socket's parent i.e GraphNode in the EditPAge.
    juce::Point<int> TopLeft = getParentComponent()->getPosition();
    TopLeft += getPosition();

    if (dir == direction::IN) {
        TopLeft += juce::Point(0, 8);
    } else {
        TopLeft += juce::Point(getWidth(), 8);
    }

    return TopLeft;
}

// helper function to find out the global coordinates
juce::Point<float> Socket::getRelToGlobal(juce::Point<int> p) {
    juce::Point<int> TopLeft = getParentComponent()->getPosition();
    TopLeft += getPosition();
    return (TopLeft+p).toFloat();
}

void Socket::acceptType(SocketDataType type) { TypesAccepted.insert(type); }

bool Socket::accepts(SocketDataType type) { return TypesAccepted.find(type) != TypesAccepted.end(); }

void Socket::update() {
    parameterController.setBounds(0, 15, getWidth(), 35);
    parameterController.update();
    addAndMakeVisible(&parameterController);
}

void *Socket::getPrevNode() {
    return (void*)(from->getParentComponent());
}

void Socket::addMenuParameterControl() {
    parameterController.createNewMenu();
}

void Socket::addSliderParameterControl(float from, float to, float default_) {
    parameterController.createTextEditor(from, to, default_);
}

void Socket::addMenuItem(juce::String name) {
    parameterController.addItemToList(name);
}

void Socket::setOutputType(SocketDataType a) {
    if (dir == direction::IN) {
        std::cout << "Trying to set the Output type of a Socket that has In direction, but why??" << "\n";
        return;
    }

    type = a;
}

SocketDataType Socket::getOutputType() {
    return type;
}

void Socket::repaintConnection() {

    if (isConnected) {
        // if the connectionPointer is not NULL,
        // fro and to should NOT be NULL.
        if (dir == direction::IN) {
            // here from would be prev socket and to would be this
            juce::Line<int> temp(from->getPivotPos(), (*(to.begin()))->getPivotPos());
            Instrument::getInstance()->connectionPositionUpdated(connectionPointer,
                                                                 temp);
        } else {
            // for output socket we redraw every connection, that is connected to.
            for (auto i = to.begin(); i != to.end(); i++) {
                // here from would be this socket and to would be the one it is to.
                // instead of updating connections from here,
                // we do it from the sockets this is connected to where these connections are
                // direction::IN.
                (*i)->repaintConnection();
            }
        }
    }

}

void Socket::deleteConnections() {
    if (dir == direction::IN) {
        deleteSocketCallBack(1, this);
        resized();
    } else {
//        repaintConnection();
        // this is because we are interfering with this to from another socket,
        // we NEED to do this.
        std::queue<Socket*> tempStore;
        for (Socket* it : to) { tempStore.push(it); }

        while (!tempStore.empty()) {
            Socket* it = tempStore.front(); tempStore.pop();
            if (it) deleteSocketCallBack(1, it);
        }

        to.clear();
    }
}


void Socket::deleteSocketCallBack(int result, Socket* thisInstance) {
    if (thisInstance == nullptr || thisInstance->dir == direction::OUT) return;

    if (result && thisInstance->isConnected) {// delete has the result 1, refactor if more options.
        thisInstance->isConnected = false;
        thisInstance->from = nullptr;
        Connection* casted = (Connection*)thisInstance->connectionPointer;
        if (casted) {
            ((Socket*)casted->getFromSocket())->deletedThisConnectionFrom(thisInstance);
            Instrument::getInstance()->connectionRemoved(thisInstance->connectionPointer);
            delete thisInstance->connectionPointer;
        }
        thisInstance->connectionPointer = nullptr;
        thisInstance->addAndMakeVisible(thisInstance->parameterController);
        thisInstance->to.clear();
    }

}

void Socket::deletedThisConnectionFrom(Socket *thatSocket) {
    if (dir == direction::IN) return; // this should not happen.
    to.erase(thatSocket);
    numberOfSocketsConnectedTo--;
    if (to.empty()) {
        from = nullptr;
        isConnected = false;
    }
    connectionPointer = nullptr;
}

void Socket::mouseDown(const juce::MouseEvent &event) {
    if (event.mods.isRightButtonDown() && isConnected) {// check if the connection is present.
        // this is because if you have multiple connections from an output node, what to delete???.
        if (dir == direction::OUT) return;

        // else
        // Show context menu
        menu.showMenuAsync(juce::PopupMenu::Options(),
                           juce::ModalCallbackFunction::forComponent(deleteSocketCallBack, this));
    } else {
        lastMousePosition = event.getPosition();
    }
}

void print_Line(juce::Line<int>& l) {
    std::cout << l.getStartX() << ", " << l.getStartY() << " -> " << l.getEndX() << ", " << l.getEndY() << '\n';
}

void Socket::mouseDrag(const juce::MouseEvent &event) {
    if (dir == direction::IN) return;

    // anything except the AudioBuffer output can be connected to multiple input sockets.
    // if (type == SocketDataType::AudioBufferFloat && isConnected) return;

    // create a new connection if it was null before.
    if (newConnection) {// getRelToGlobal(event.getPosition()).toFloat()
        juce::Line<int> temp(this->getPivotPos(), getRelToGlobal(event.getPosition()).toInt());
        Instrument::getInstance()->connectionInst(newConnection, temp);
    // print_Line(temp);
    } else {
        newConnection = new Connection();
    }

}

void Socket::connected(Socket *otherPointer, Connection* connection) {
    isConnected = true;
    connectionPointer = connection;

    // nothing to set for output node here.
    // (it can be multiple to`s some time)
    if (dir == direction::IN) {
        from = otherPointer;
        to.clear();
        to.insert(this);
        removeChildComponent(&parameterController);

        // this is the last function call that happens after a connection is confirmed in this class.
        Instrument::getInstance()->connectionAdded(connection);

        resized();
    } else {
        from = this;
        to.insert(otherPointer);
//        connection->setConnectionParams(type, 0, 0);
    }

    numberOfSocketsConnectedTo++;
    repaintConnection();
}

void Socket::resized() {
    if (getParentComponent()) (getParentComponent()->resized());
}

//''''''''''''
// API API API API API API API : PAIN-KILLERS
float Socket::getValue() {
    // if it is connected we get the value from the previous node,
    // else we just return what the value in parameter control is.
    if (connectionPointer) return connectionPointer->getFloatValue();
    else return parameterController.getValue();
}

juce::AudioBuffer<float> *Socket::getBufferPointer() {
    if (connectionPointer) return connectionPointer->getBufferPointer();
    return nullptr; // this is left for debugging, normally you should ensure
    // not to have this function called in the callback in the templated function
    // for the particular case called in the `processGraphNode`.
}

juce::MidiBuffer *Socket::getMidiMessage() {
    if (connectionPointer) return connectionPointer->midiMessagePointer;
    return nullptr;
}


void Socket::setFloatValue(float f) {
    for (Socket* i : to) i->connectionPointer->setFloatValue(f);
}

void Socket::setBufferPointer(juce::AudioBuffer<float> *bufferPointer) {
    for (Socket* i : to) i->connectionPointer->setBufferPointer(bufferPointer);
}

void Socket::setMidiMessagePointer(juce::MidiBuffer *midiMessagePointer) {
    for (Socket* i : to) i->connectionPointer->setMidiMessagePointer(midiMessagePointer);
}

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

Socket::~Socket() {
    deleteConnections();
}

void Socket::mouseUp(const juce::MouseEvent &event) {
    // out to out is not accepted.
    if (!newConnection) return;

    Socket* ComponentUnderMouse = Instrument::getInstance()->getComponentInGraphPage(getRelToGlobal(event.getPosition()));

    // at this point the component we got is an Input Socket,
    // now we need to check if it accepts the type given out by the
    // socket that is being connected to.

    // TODO : Check for loops, please.
    if (ComponentUnderMouse && // 2nd check will not be done if this was false, so no nullptr errors.
        ComponentUnderMouse->dir == direction::IN &&
        ComponentUnderMouse->accepts(type) &&
        !ComponentUnderMouse->isConnected) {
        newConnection->confirmConnection(static_cast<void*>(getParentComponent()),
                                         static_cast<void*>(this),
                                         static_cast<void*>(ComponentUnderMouse->getParentComponent()),
                                         static_cast<void*>(ComponentUnderMouse));

        // This with that, that with this.
        connected(ComponentUnderMouse, newConnection);
        ComponentUnderMouse->connected(this, newConnection);

        newConnection = nullptr;
    } else { // if the dynamic cast returned a nullptr.
        // if anything of it did not work out, delete the node and act line nothing ever happened.
        Instrument::getInstance()->connectionInitFail();
        if (newConnection) delete newConnection;
        newConnection = nullptr;
    }


}


