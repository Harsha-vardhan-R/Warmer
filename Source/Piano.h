/*
  ==============================================================================

    Piano.h
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Piano : public juce::Component {
public:

    Piano();
    ~Piano() override;

    void paint(juce::Graphics& g) override;
    void resized() override;


    // overridden function from juce::KeyListener
    //bool keyPressed(const juce::KeyPress& key) override;

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


        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override {};
        void mouseUp(const juce::MouseEvent& event) override;

        bool keyPressed(const juce::KeyPress&, juce::Component*) override;
        bool keyStateChanged(bool isKeyDown, juce::Component* c) override {
            if (isKeyDown == false) {
                WhiteKeyUp(25);
                repaint();
                return true;
            }
            return false;
        };


        // for callbacks from other classes like from MIDI and keyboard.
//        std::function<void(int)> WhiteKeyDownCallback;
//        std::function<void(int)> WhiteKeyUpCallback;
//        std::function<void(int)> BlackKeyDownCallback;
//        std::function<void(int)> BlackKeyUpCallback;


    private:
        int offset;

        // The white keys that are presently pressed.
        std::set<int> pressedWhiteKeys;
        std::set<int> pressedBlackKeys;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlayKeyPaint)
    };


private:
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

    std::unique_ptr<Piano::OverlayKeyPaint> overlayPainter;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Piano)
};