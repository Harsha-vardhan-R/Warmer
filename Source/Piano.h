/*
  ==============================================================================

    Piano.h
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Wheels.h"


class Piano : public juce::Component {
public:

    Piano();
    ~Piano() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setOffset(int n) {
        offset = n;
    }


    /*
        This is an overlay which exactly lies over the piano component,
        because we need not draw every key every time a key is pressed or released ,
        this just paints over the top of the keys leaving the remaining unchanged.
    */
    class OverlayKeyPaint : public juce::Component,
                            public juce::KeyListener {
    public :

        OverlayKeyPaint();
        ~OverlayKeyPaint() override {};

        void paint(juce::Graphics& g) override;
        void resized() override;

        void WhiteKeyDown(int keyIndex);
        void WhiteKeyUp(int keyIndex);

        void BlackKeyDown(int keyIndex);
        void BlackKeyUp(int keyIndex);

        bool keyPressed(const juce::KeyPress&, juce::Component*) override;
        bool keyStateChanged(bool isKeyDown, juce::Component*) override;

        void setOffset(int n) {
            offset = n;
        }



        // for callbacks from other classes like from MIDI and keyboard.
//        std::function<void(int)> WhiteKeyDownCallback;
//        std::function<void(int)> WhiteKeyUpCallback;
//        std::function<void(int)> BlackKeyDownCallback;
//        std::function<void(int)> BlackKeyUpCallback;

        void setTransposeLevel(int level) {
            transposeLevel = level;
        }

    private:
        int offset;

        int transposeLevel;

        // The white keys that are presently pressed.
        std::set<int> pressedWhiteKeys;
        std::set<int> pressedBlackKeys;

        std::set<int> pressedWhiteKeyCodes;
        std::set<int> pressedBlackKeyCodes;

//        int pos_to_type[7] = {1, 2, 3, 1, 2, 2, 3};


        // images to overlay.
        juce::Image WImage = juce::ImageFileFormat::loadFrom(juce::File("./Assets/KeyPressedImages/PianoFullImageWhitePressed_.png"));
        juce::Image Bimage = juce::ImageFileFormat::loadFrom(juce::File("./Assets/KeyPressedImages/B2.png"));

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlayKeyPaint)
    };

    // Should be visible.

    std::unique_ptr<Piano::OverlayKeyPaint> overlayPainter;


private:

    int offset;

    const int blackKeysPosition[37] = { 1,  2,  4,  5,  6,
                                        8,  9, 11, 12, 13,
                                       15, 16, 18, 19, 20,
                                       22, 23, 25, 26, 27,
                                       29, 30, 32, 33, 34,
                                       36, 37, 39, 40, 41,
                                       43, 44, 46, 47, 48,
                                       50, 51};

    const int keyColour[87] = {
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 0, 1
    };



    juce::Image PianoImage = juce::ImageFileFormat::loadFrom(juce::File("./Assets/KeyPressedImages/piano_full_unpressed.png"));


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Piano)
};