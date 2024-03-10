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

//==============================================================================
/*
*/
class Socket  : public juce::Component {
public:

    direction dir;
    std::set<SocketDataType> TypesAccepted;
    juce::String name;

    Socket(juce::String name, direction dir, bool isMust = false) {
        this->name = name;
        this->dir = dir;
        this->isMust = isMust;
        posx = 0; posy = 0; wid = 0;
    }

    ~Socket() override {}

    void paint (juce::Graphics& g) override {
        g.fillRoundedRectangle(posx, posy, wid, wid, ((float)wid)/2.0);
    }

    void bound(int posx, int posy, int wid) {
        this->posx = posx;
        this->posy = posy;
        this->wid = wid;
    }

    void resized() override {
        setBounds(posx, posy, wid, wid);
    }

private:
    int posx, posy, wid;

    bool isMust;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
};
