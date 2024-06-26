/*
  ==============================================================================

    GraphNode.cpp
    Created: 8 Apr 2024 4:27:10pm
    Author:  harshavardhan

  ==============================================================================
*/

#include "GraphNode.h"

#include <utility>
#include "../Instrument.h"

Instrument* instrument_ptr = nullptr;


juce::String setOfNodesToString(const std::set<GraphNode::Socket*>& set_) {
    int i = 0;

    juce::String t;

    t += juce::String("(");

    for (GraphNode::Socket* a : set_) {
        if (i) t += juce::String(",");
        t += juce::String("(");
        t += juce::String(juce::String(reinterpret_cast<uint64_t>(a->getParent())));
        t += juce::String(",");
        t += juce::String(juce::String(a->getSocketsIndexInParent()));
        t += juce::String(")");
        i++;
    }

    t += juce::String(")");

    return t;

}

juce::XmlElement *GraphNode::makeXML() {
    auto* node = new juce::XmlElement("GraphNode");
    node->setAttribute("Name" , name);
    mini_makeXML(node);

    for (auto i : OutputSockets) {
        auto* soc = new juce::XmlElement("Socket");
        soc->setAttribute("Direction", "OUT");
        soc->setAttribute("Connections", setOfNodesToString(i->to));
        soc->addChildElement(i->makeXML());
        node->addChildElement(soc);
    }

    for (auto i : InputSockets) {
        auto* soc = new juce::XmlElement("Socket");
        soc->setAttribute("Direction", "IN");
        soc->addChildElement(i->makeXML());
        node->addChildElement(soc);
    }

    return node;
}

void GraphNode::parseXMLChildren(juce::XmlElement* x) {
    //Profiler("Graph Node");
    
    // Parse any saved state the child is in before doing this,
    // please make sure that the number of sockets when saved should be 
    // equal to the the number of sockets after this function returnes.
    mini_parseXMLChildren(x);

    int a = 0;
    for (auto* Child = x->getFirstChildElement(); Child; Child = Child->getNextElement()) {
        if (Child->hasTagName("Socket")) {
            if (Child->compareAttribute("Direction", "OUT", true)) continue;
            // for the input Socket Initialise the parameters, so call
            // parse XML on the Sockets.
            // The Sockets will be in the same order as they are saved as order.
            InputSockets[a]->parseXMLChildren(Child);
            a++;
        } else {
            std::cout << "ERROR, Corrupt file" << "\n";
            return;
        }
    }

}

void makeConnections(GraphNode::Socket* s, juce::String str, const std::unordered_map<juce::String, GraphNode*>& map_) {
    juce::String trimmedStr = str.substring(1, str.length() - 1);

    // No Connections.
    if (trimmedStr.isEmpty()) return;

    juce::StringArray nodesArray;
    int startIndex = 0;

    while (startIndex < trimmedStr.length()) {
        int endIndex = trimmedStr.indexOf(startIndex, ")");
        if (endIndex == -1) {
            break;
        }
        juce::String nodeStr = trimmedStr.substring(startIndex, endIndex + 1);
        nodesArray.add(nodeStr);
        startIndex = endIndex + 2; // Skip the comma after ')'
    }

    for (const juce::String& nodeStr : nodesArray) {
        if (nodeStr[0] != '(' || nodeStr[nodeStr.length() - 1] != ')') {
            std::cerr << "Invalid Connections format" << std::endl;
            continue;
        }

        // Remove node parentheses
        juce::String nodeContent = nodeStr.substring(1, nodeStr.length() - 1);

        // Split by comma to get parent unique number and socket index.
        juce::StringArray nodeParts = juce::StringArray::fromTokens(nodeContent, ",", "");
        if (nodeParts.size() != 2) {
            std::cerr << "Invalid node parts" << std::endl;
            continue;
        }

        const juce::String& parentAddress = nodeParts[0];
        int socketIndex = nodeParts[1].getIntValue();

        auto it = map_.find(parentAddress);
        if (it != map_.end()) {
            GraphNode* toNode = it->second;
            GraphNode::Socket* toSocket = toNode->getInputSocketAtIndex(socketIndex);

            // Create the connection
            auto* newConnection = new Connection();

            newConnection->confirmConnection(static_cast<void*>(s->getParentComponent()),
                                             static_cast<void*>(s),
                                             static_cast<void*>(toNode),
                                             static_cast<void*>(toSocket));

            // This with that, that with this.
            s->connected(toSocket, newConnection, false);
            toSocket->connected(s, newConnection, false);
        } else {
            std::cerr << "Parent address not found in the map" << std::endl;
        }
    }
}




void GraphNode::XMLParseHelper(juce::XmlElement* xmlEle, const std::unordered_map<juce::String, GraphNode*>& map_) {

    int b = 0;
    for (auto* Child = xmlEle->getFirstChildElement(); Child; Child = Child->getNextElement()) {
        if (Child->hasTagName("Socket")) {
            if (!Child->compareAttribute("Direction", "OUT", true)) break;
            // for the output Sockets create the connections.
            juce::String s = Child->getStringAttribute("Connections");
            makeConnections(OutputSockets[b], s, map_);
            b++;
        } else {
            std::cout << "ERROR, Corrupt file" << "\n";
            return;
        }
    }

}

std::vector<GraphNode*> GraphNode::getDependencies() {
    std::vector<GraphNode*> output;

    for (auto i : InputSockets) {
        if (i->getPrevNode() != nullptr) output.push_back(static_cast<GraphNode*>(i->getPrevNode()));
    }

    return output;
}


std::set<GraphNode*> GraphNode::getDependents() {
    std::set<GraphNode*> output;

    for (auto i : OutputSockets) {
        for (Socket* j : i->to) output.insert(static_cast<GraphNode*>(j->getParentComponent()));
    }

    return output;
}

std::set<GraphNode*> GraphNode::getAudioBufferDependencies() {
    std::set<GraphNode*> output;

    // we return the set of nodes connected to the first socket of
    // the type AudioBufferFloat in OutputSockets.
    for (Socket* i : OutputSockets) {
        if (i->getOutputType() == SocketDataType::AudioBufferFloat) {
            for (Socket *j: i->to) output.insert(static_cast<GraphNode*>(j->getParentComponent()));
            break;
        }
    }

    return output;
}


GraphNode::GraphNode(juce::String name, int pos_x, int pos_y) {
    instrument_ptr = Instrument::getInstance();

    setLookAndFeel(&style);

    this->name = std::move(name);

    UIWidth = 0;
    UIHeight = 0;

    setBounds(pos_x, pos_y, UIWidth, UIHeight);

    canBeDeleted = true; // for all the nodes by default, will be set to false in the input and output nodes.


    menu.addItem(1, "Delete Node");
    menu.setLookAndFeel(&menu_style);

    Component::setViewportIgnoreDragFlag(true);

    resized();
    repaint();
}

// returns true if all the Input Sockets that are `isMust` are connected.
bool GraphNode::allGood() {
    for (auto i : InputSockets) {
        // if it is a must but not connected we are going to return false.
        if (i->isMust && !(i->isThisConnected())) {
            return false;
        }
    }

    for (auto i : OutputSockets) {
        // if it is a must but not connected we are going to return false.
        if (i->isMust && !(i->isThisConnected())) {
            return false;
        }
    }

    return true;
}

void GraphNode::deleteNodeCallback(int result, GraphNode* graphNodeInstance) {
    if (result == 1) {
        graphNodeInstance->deleteAllConnectionsToAndFromNode();
        instrument_ptr->nodeDeleted(graphNodeInstance);
    }
}

// for moving and delete menu.
void GraphNode::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        if (!canBeDeleted) return;
        // Show context menu
        menu.showMenuAsync(juce::PopupMenu::Options(),
                           juce::ModalCallbackFunction::forComponent(deleteNodeCallback, this));
    } else {
        lastMouseDownPosition = event.getPosition();
//        resized();
    }
}

void GraphNode::repaintAllInputConnectionsToNode() {
    for (Socket* i : InputSockets) {
        i->repaintConnection();
    }

    instrument_ptr->triggerGraphPageRepaint();
}

void GraphNode::mouseDrag(const juce::MouseEvent& event) {
    juce::Point<int> delta = event.getPosition() - lastMouseDownPosition;
    auto newBounds = getBounds().translated(delta.getX(), delta.getY());
//    std::cout << newBounds.getX() << " " << newBounds.getY() << " " << newBounds.getWidth() << " " << newBounds.getHeight() << "\n";
    setBounds(newBounds);

    // mouse drag triggers the repainting of connections to the node.
    for (Socket* i : InputSockets) {
        i->repaintConnection();
    }

    for (Socket* i : OutputSockets) {
        i->repaintConnection();
    }

    instrument_ptr->triggerGraphPageRepaint();

}

void GraphNode::mouseUp(const juce::MouseEvent &event) {
    instrument_ptr->triggerGraphPageRepaint();
}

void GraphNode::deleteAllConnectionsToAndFromNode() {

    // mouse drag triggers the repainting of connections to the node.
    for (Socket* i : InputSockets) {
        i->deleteConnections();
    }

    for (Socket* i : OutputSockets) {
        i->deleteConnections();
    }

}

// Draws the common parts for all the nodes,
// called before drawing the respective node related content.
void GraphNode::paintBasic(juce::Graphics& g) {
    auto bounds = getLocalBounds();

    g.setColour(GraphNodeBackgroundColourID);
    g.fillRect(bounds);

    g.setColour(GraphNodeHeadingColourID);
    juce::Rectangle<int> topTextArea(0, 0, getWidth(), 20);
    g.fillRect(topTextArea);

    g.setColour(juce::Colours::lightgrey);
    g.drawRect(bounds);

    g.setColour(GraphNodeHeadingTextID);
    g.drawText(name, topTextArea.toFloat(), juce::Justification::centred);

}

// sets all the sockets in both input and output socket arrays to
// addAndMakeVisible
void GraphNode::makeAllSocketsVisible() {
    resized(); // set the bounds.

    int lll = 0;

    // draw the sockets.
    for (Socket* i : InputSockets) {
        addAndMakeVisible(i);
        i->update();
        i->setParent(this);
        i->thisSocketsIndexIs(lll);
        lll++;
    }

    for (Socket* i : OutputSockets) {
        addAndMakeVisible(i);
        i->update();
    }

}

void GraphNode::paint(juce::Graphics& g) {
    paintBasic(g);
}

void GraphNode::resized() {
    UIWidth = 120;

    int index = 0;
    int end = 30;
    // setting the bounds for each socket.
    // output sockets are displayed at the top, followed by input sockets.
    for (auto i : OutputSockets) {
        i->setBounds(0, 30+(index*20), UIWidth, 20);
        end = (index*20)+50;
        index++;
    }

    end += 5;

    index = 0;
    for (auto i : InputSockets) {
        int height = i->getTotHeight();
        i->setBounds(0, end, UIWidth, height);
        end += height;
        index++;
    }


    UIHeight = end + 10;
    setBounds(getX(), getY(), UIWidth, UIHeight);

}

GraphNode::~GraphNode() {
    setLookAndFeel(nullptr);
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
////////// Function definition of the Socket /////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


GraphNode::Socket::Socket(juce::String name, direction dir, bool isMust) : parameterController(this) {
    this->name = name;
    this->dir = dir;
    this->isMust = isMust;

    menu.addItem(1, "Remove Connection");
    menu.setLookAndFeel(&menu_style);
}


void GraphNode::Socket::paint(juce::Graphics &g)  {
    // Small square
    if (isMust) g.setColour(GraphNodeConnectionBoxMustID);
    else g.setColour(GraphNodeConnectionBoxID);

    // one for input socket and other for output socket check.
    if (dir == direction::IN) {
        if (!TypesAccepted.empty()) g.fillRect(0, 5, 5, 5);

        int a = TypesAccepted.count(SocketDataType::AudioBufferFloat);
        int b = TypesAccepted.count(SocketDataType::MIDI);
        int c = TypesAccepted.count(SocketDataType::Floating);

        // set decide the colour for the text.
        juce::Colour TextColourHere = experimental[a][b][c];

        juce::Rectangle<int> bound(10, 0, getWidth()-10, 15);
        g.setColour(TextColourHere);
        g.drawText(name, bound, juce::Justification::centredLeft);
    } else {
        if (type != SocketDataType::NULLType) g.fillRect(getWidth()-5, 5, 5, 5);

        juce::Rectangle<int> bound(0, 0, getWidth()-10, 15);

        if (type == SocketDataType::AudioBufferFloat) {
            g.setColour(experimental_colour_single[0]);
        } else if (type == SocketDataType::MIDI) {
            g.setColour(experimental_colour_single[1]);
        } else {
            g.setColour(experimental_colour_single[2]);
        }

        g.drawText(name, bound, juce::Justification::centredRight);
    }

}

juce::Point<int> GraphNode::Socket::getPivotPos() {
    // position of the socket's parent i.e GraphNode in the EditPAge.
    juce::Point<int> TopLeft = getParentComponent()->getPosition();
    TopLeft += getPosition();

    int comp = (dir == direction::IN);
    TopLeft += comp * juce::Point(0, 8) + (1-comp) * juce::Point(getWidth(), 8);

    return TopLeft;
}

// helper function to find out the global coordinates
juce::Point<float> GraphNode::Socket::getRelToGlobal(juce::Point<int> p) {
    juce::Point<int> TopLeft = getParentComponent()->getPosition();
    TopLeft += getPosition();
    return (TopLeft+p).toFloat();
}

void GraphNode::Socket::acceptType(SocketDataType type) { TypesAccepted.insert(type); }

bool GraphNode::Socket::accepts(SocketDataType type) { return TypesAccepted.find(type) != TypesAccepted.end(); }

void GraphNode::Socket::update() {
    parameterController.setBounds(0, 15, getWidth(), getTotHeight()-15);
    parameterController.update();
    addAndMakeVisible(&parameterController);
}

void* GraphNode::Socket::getPrevNode() {
    return (void*)(from->getParentComponent());
}

void GraphNode::Socket::addMenuParameterControl() {
    parameterController.createNewMenu();
}

void GraphNode::Socket::addSliderParameterControl(float from, float to, float default_) {
    parameterController.createTextEditor(from, to, default_);
}

void GraphNode::Socket::addMenuItem(juce::String name) {
    parameterController.addItemToList(name);
}

void GraphNode::Socket::selectMenuItem(int val) {
    parameterController.selectMenuItem(val);
}

void GraphNode::Socket::setOutputType(SocketDataType a) {
    if (dir == direction::IN) {
        std::cout << "Trying to set the Output type of a Socket that has In direction, but why??" << "\n";
        return;
    }

    type = a;
}


SocketDataType GraphNode::Socket::getOutputType() {
    return type;
}

SocketDataType GraphNode::Socket::getConnectionType() {
    if (dir == direction::OUT) {
        std::cout << "The function getConnectionType is meant to be called on an input node to know the current type of connection" << "\n";
        return SocketDataType::NULLType;

    }

    if (from) {
        return from->getOutputType();
    } else {
        return SocketDataType::NULLType;
    }
}

int GraphNode::Socket::repaintConnection() {

    //Profiler("Socket Position update");

    if (isConnected) {
        // if the connectionPointer is not NULL,
        // fro and to should NOT be NULL.
        if (dir == direction::IN) {
            // here from would be prev socket and to would be this
            juce::Line<int> temp(from->getPivotPos(), (*(to.begin()))->getPivotPos());
            Instrument::instancePtr->connectionPositionUpdated(connectionPointer,
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
        return 1;
    }
    return 0;
}

void GraphNode::Socket::deleteConnections() {
    if (dir == direction::IN) {
        deleteSocketCallBack(1, this);
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


void GraphNode::Socket::deleteSocketCallBack(int result, Socket* thisInstance) {
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
        thisInstance->resized();
    }

    // here we trigger this function because deleting a node should repaint all the connections,
    // the height at which they are may change.
    if (thisInstance->parentNodePointer) thisInstance->parentNodePointer->repaintAllInputConnectionsToNode();

}

void GraphNode::Socket::deletedThisConnectionFrom(Socket *thatSocket) {
    if (dir == direction::IN) return; // this should not happen.
    to.erase(thatSocket);
    numberOfSocketsConnectedTo--;
    if (to.empty()) {
        from = nullptr;
        isConnected = false;
    }
    connectionPointer = nullptr;
}

void GraphNode::Socket::mouseDown(const juce::MouseEvent &event) {
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

void GraphNode::Socket::mouseDrag(const juce::MouseEvent &event) {
    if (dir == direction::IN) return;

    //Profiler("Socket mouse drag");

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

void GraphNode::Socket::connected(Socket *otherPointer, Connection* connection, bool flag) {
    isConnected = true;
    connectionPointer = connection;

    // nothing to set for output node here.
    // (it can be multiple to`s some time)
    if (dir == direction::IN) {
        from = otherPointer;
        to.clear();
        to.insert(this);

        if (collapseOnConnection) removeChildComponent(&parameterController);

        resized();

        // this is the last function call that happens after a connection is confirmed in this class.
        if (flag) Instrument::getInstance()->connectionAdded(connection);

        if (parentNodePointer) parentNodePointer->repaintAllInputConnectionsToNode();

        int a = TypesAccepted.count(SocketDataType::AudioBufferFloat);
        int b = TypesAccepted.count(SocketDataType::MIDI);
        int c = TypesAccepted.count(SocketDataType::Floating);

        // set decide the colour for the text.
        connection->toColour = experimental[a][b][c];
    } else {
        from = this;
        to.insert(otherPointer);

        if (type == SocketDataType::AudioBufferFloat) {
            connection->fromColour = experimental_colour_single[0];
        } else if (type == SocketDataType::MIDI) {
            connection->fromColour = experimental_colour_single[1];
        } else {
            connection->fromColour = experimental_colour_single[2];
        }
    }

    numberOfSocketsConnectedTo++;
    repaintConnection();
}

void GraphNode::Socket::resized() {
    if (getParentComponent()) (getParentComponent()->resized());
}

//''''''''''''
// API API API API API API API : PAIN-KILLERS
float GraphNode::Socket::getValue() {
    // if it is connected we get the value from the previous node,
    // else we just return what the value in parameter control is.
    if (connectionPointer && collapseOnConnection) return connectionPointer->getFloatValue();
    else return parameterController.getValue();
}

juce::AudioBuffer<float> *GraphNode::Socket::getBufferPointer() {
    if (connectionPointer) return connectionPointer->getBufferPointer();
    return nullptr; // this is left for debugging, normally you should ensure
    // not to have this function called in the callback in the templated function
    // for the particular case called in the `processGraphNode`.
}

juce::MidiBuffer *GraphNode::Socket::getMidiBuffer() {
    if (connectionPointer) return connectionPointer->midiMessagePointer;
    return nullptr;
}


void GraphNode::Socket::setFloatValue(float f) {
    for (Socket* i : to) i->connectionPointer->setFloatValue(f);
}

void GraphNode::Socket::setBufferPointer(juce::AudioBuffer<float> *bufferPointer) {
    for (Socket* i : to) i->connectionPointer->setBufferPointer(bufferPointer);
}

void GraphNode::Socket::setMidiBufferPointer(juce::MidiBuffer *midiMessagePointer) {
    for (Socket* i : to) i->connectionPointer->setMidiMessagePointer(midiMessagePointer);
}

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

GraphNode::Socket::~Socket() {
    deleteConnections();
}

void GraphNode::Socket::mouseUp(const juce::MouseEvent &event) {
    // out to out is not accepted.
    if (!newConnection) return;

    Socket* ComponentUnderMouse = Instrument::getInstance()->getComponentInGraphPage(getRelToGlobal(event.getPosition()));

    // at this point the component we got is an Input Socket,
    // now we need to check if it accepts the type given out by the
    // socket that is being connected to.


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

void GraphNode::Socket::setParameterCtrlNoCollapseOnConnection() {
    collapseOnConnection = false;
}

void GraphNode::Socket::addEnvParameterControl() {
    parameterController.addEnvCtrlComponent();
}

void GraphNode::Socket::addFilterDisplayControl() {
    parameterController.addFilterDisplayComponent();
}
