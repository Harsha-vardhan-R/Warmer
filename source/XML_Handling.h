//
// Created by harshavardhan on 13/6/24.
//

#pragma once
#include <juce_data_structures/juce_data_structures.h>

class makeParseXML {
public:

    // captures the present wanted configuration,
    // returns a pointer to the created XMLElement inside,
    // pointers are taken ownership by the parent of the
    // XMLChild, so no need to worry.
    virtual juce::XmlElement* makeXML() = 0;

    // this is where the processing happens.
    // for example if the GraphPage is processing this
    // it will read and create the nodes,
    // and if the node is processing it will read and create sockets
    // and other related stuff.
    virtual void parseXMLChildren(juce::XmlElement* x) = 0;

private:

};
