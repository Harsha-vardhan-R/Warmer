/*
  ==============================================================================

    Socket.h
    Created: 10 Mar 2024 3:35:29pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Socket  : public juce::Component
{
public:
    Socket() {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

    }

    ~Socket() override {

    }

    void paint (juce::Graphics& g) override
    {
        g.drawRoundedRectangle();
    }

    void resized() override {

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Socket)
};
