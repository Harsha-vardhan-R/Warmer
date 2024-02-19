/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"

// Fixed positions of the white keys(without the offset).
const int WhiteKeyPositions[53] = {
        1,35,69,103,137,171,205,239,
        273,307,341,375,409,443,477,
        511,545,579,613,647,681,715,
        749,783,817,851,885,919,953,
        987,1021,1055,1089,1123,1157,1191,
        1225,1259,1293,1327,1361,1395,1429,
        1463,1497,1531,1565,1599,1633,1667,
        1701,1735,1769
};

const int blackKeyPianoPosition[37] = {
        23 ,  57 ,125 ,159 ,193 ,
        261 ,295 ,363 ,397 ,431 ,
        499 ,533 ,601 ,635 ,669 ,
        737 ,771 ,839 ,873 ,907 ,
        975 ,1009 ,1077 ,1111 ,1145 ,
        1213 ,1247 ,1315 ,1349 ,1383 ,
        1451 ,1485 ,1553 ,1587 ,1621 ,
        1689 ,1723
};

Piano::Piano() {
    overlayPainter.reset(new Piano::OverlayKeyPaint());
    overlayPainter.get()->setWantsKeyboardFocus(true);
    addAndMakeVisible(overlayPainter.get());


    resized();
}

Piano::~Piano() {}

void Piano::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::grey);

    int pianoWhiteKeyWidth = 33;
    int pianoBlackKeyWidth = 20;
    char c = '0';

    int offset = 885-(getWidth()/2);
    int firstVisibleWhiteKey = offset / pianoWhiteKeyWidth-1;
    int lastVisibleWhiteKey = juce::jmin(53, (getWidth() + offset) / pianoWhiteKeyWidth+1);

    // draw white keys.
    g.setColour(juce::Colour(0xffeee3e7));
    for (int i = firstVisibleWhiteKey; i < lastVisibleWhiteKey; ++i) {
        g.fillRect(WhiteKeyPositions[i]-offset, 0, pianoWhiteKeyWidth, 126);
    }

    // draw the octave numbers for all the c keys.
    g.setColour(juce::Colours::grey);
    for (int i = 0; i < 53; i+=7, c++) {
            juce::String noteC("Cz");
            g.drawText(noteC.replaceCharacter('z', c), WhiteKeyPositions[i]-offset, 100, 32, 20, juce::Justification::centredBottom);
    }

    // draw the black keys.
    g.setColour(juce::Colours::darkgrey);
    for (int i = 0; i < 37; i++) {
        if (blackKeyPianoPosition[i] - offset > getWidth()) break;
        g.fillRect(blackKeyPianoPosition[i] - offset, 0, pianoBlackKeyWidth, 90);
    }

    g.setColour(juce::Colours::grey);
    g.fillRect(0, 0, getWidth(), 3);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void Piano::resized() {
    setBounds(150, 20 ,getParentWidth()-150, getParentHeight() - 20);
    if (overlayPainter.get() != nullptr) overlayPainter.get()->resized();
}

void Piano::OverlayKeyPaint::mouseDown(const juce::MouseEvent &event) {
    WhiteKeyDown(((event.x+offset)/34));
}

void Piano::OverlayKeyPaint::mouseUp(const juce::MouseEvent &event) {
    WhiteKeyUp(((event.x+offset)/34));
}

// Overlay method implementations
// Mostly used from the callbacks from the instrument.

Piano::OverlayKeyPaint::OverlayKeyPaint() {
    pressedWhiteKeys = {};
    pressedBlackKeys = {};

    this->addKeyListener(this);

    resized();
}

void Piano::OverlayKeyPaint::WhiteKeyDown(int keyIndex) {
    this->pressedWhiteKeys.insert(keyIndex);
    repaint();
}

void Piano::OverlayKeyPaint::WhiteKeyUp(int keyIndex) {
    // does not raise an exception if not found, so no need to handle any kind of exception.
    this->pressedWhiteKeys.erase(keyIndex);
    repaint();
}

void Piano::OverlayKeyPaint::BlackKeyDown(int keyIndex) {
    this->pressedBlackKeys.insert(keyIndex);
    repaint();
}

void Piano::OverlayKeyPaint::BlackKeyUp(int keyIndex) {
    this->pressedBlackKeys.erase(keyIndex);
    repaint();
}

void fillGradient(juce::Graphics& g, const juce::Colour& colour1, const juce::Colour& colour2,
                  int x, int y, int width, int height) {
    juce::ColourGradient gradient(colour1, x, y, colour2, x + width, y + height, false);
    g.setGradientFill(gradient);
    g.fillRect(x, y, width, height);
}

void Piano::OverlayKeyPaint::paint(juce::Graphics &g) {

    int pianoWhiteKeyWidth = 33;
    int pianoBlackKeyWidth = 20;

    // Draw the overlay for the pressed white keys.
    // Not looking for out of bounds, handled by the `fillRoundedRectangle`
    g.setColour(juce::Colour(0xffeee3e7));
    for (auto i = pressedWhiteKeys.begin(); i != pressedWhiteKeys.end(); ++i) {
        fillGradient(g, juce::Colour(0xffeee3e7), juce::Colour(0x33f6abb6),
                     WhiteKeyPositions[*i] - offset, 90, pianoWhiteKeyWidth, 33);
    }

    g.setColour(juce::Colours::darkgrey);
    for (auto i = pressedBlackKeys.begin(); i != pressedBlackKeys.end(); ++i) {
        std::cout << *i << "\n";
        g.fillRoundedRectangle(blackKeyPianoPosition[*i] - offset, 80, pianoBlackKeyWidth, 13, 3.0f);
    }

}

void Piano::OverlayKeyPaint::resized() {
    if (this->getParentComponent() != nullptr) setBounds(this->getParentComponent()->getLocalBounds().reduced(2));
    this->offset = 885-(getWidth()/2);
}

bool Piano::OverlayKeyPaint::keyPressed(const juce::KeyPress& k, juce::Component* c) {

    if (k == juce::KeyPress::spaceKey) {
        WhiteKeyDown(25);
        repaint();
        return true;
    }

    return false;
}