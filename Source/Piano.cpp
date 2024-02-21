/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"
#include "Wheels.h"

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


int KeysAndCorrespondingKeyCodes[150];
int KeyMap[12] = {97, 115, 100, 102, 103,
                  104, 106};
int PianoKeyMap[12] = {21, 22, 23, 24, 25,
                       26, 27};
int BlackKeysAndCorrespondingKeyCodes[150];
int BlackKeyMap[12] = {119, 101, 116,
                  121, 117};
int BlackPianoKeyMap[12] = {15, 16, 17,
                       18, 19};

Piano::Piano() {
    overlayPainter.reset(new Piano::OverlayKeyPaint());
    overlayPainter.get()->setWantsKeyboardFocus(true);
    addAndMakeVisible(overlayPainter.get());

    resized();
}

Piano::~Piano() {}

void Piano::paint(juce::Graphics &g) {
    int offset = 885-(getWidth()/2);
    g.drawImage(PianoImage, 0 , 0, getWidth(), getHeight(),
                offset, 0, getWidth(), getHeight(), false);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
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

    for (int i = 0; i < 150; i++) {
        KeysAndCorrespondingKeyCodes[i] = -1;
    }
    int i = 0;
    for (auto j : KeyMap) {
        KeysAndCorrespondingKeyCodes[j] = PianoKeyMap[i];
        i++;
    }
    for (int i = 0; i < 150; i++) {
        BlackKeysAndCorrespondingKeyCodes[i] = -1;
    }
    i = 0;
    for (auto j : BlackKeyMap) {
        BlackKeysAndCorrespondingKeyCodes[j] = BlackPianoKeyMap[i];
        i++;
    }

    this->addKeyListener(this);

    resized();
}

void Piano::OverlayKeyPaint::WhiteKeyDown(int keyIndex) {
    this->pressedWhiteKeys.insert(keyIndex + (transposeLevel*7));
}

void Piano::OverlayKeyPaint::WhiteKeyUp(int keyIndex) {
    // does not raise an exception if not found, so no need to handle any kind of exception.
    this->pressedWhiteKeys.erase(keyIndex + (transposeLevel*7));
    repaint();
}

void Piano::OverlayKeyPaint::BlackKeyDown(int keyIndex) {
    this->pressedBlackKeys.insert(keyIndex + (transposeLevel*5));
    repaint();
}

void Piano::OverlayKeyPaint::BlackKeyUp(int keyIndex) {
    this->pressedBlackKeys.erase(keyIndex + (transposeLevel*5));
}

void fillGradient(juce::Graphics& g, const juce::Colour& colour1, const juce::Colour& colour2,
                  int x, int y, int width, int height) {
    juce::ColourGradient gradient(colour1, x, y, colour2, x + width, y + height, false);
    g.setGradientFill(gradient);
    g.fillRect(x, y, width, height);
}

int count = 0;
void Piano::OverlayKeyPaint::paint(juce::Graphics &g) {

    std::cout << transposeLevel << " " << count++ << " \n";

    int pianoWhiteKeyWidth = 33;
    int pianoBlackKeyWidth = 20;
    int num;

    // Draw the overlay for the pressed white keys.
    for (auto i = pressedWhiteKeys.begin(); i != pressedWhiteKeys.end(); ++i) {
        g.drawImage(WImage, WhiteKeyPositions[*i] - offset - 1, 0, 34, 130,
                    WhiteKeyPositions[*i], 1, 34, 130, false);
    }

    for (auto i = pressedBlackKeys.begin(); i != pressedBlackKeys.end(); ++i) {
        g.drawImage(Bimage, blackKeyPianoPosition[*i] - offset, 2, 20, 89,
                    0, 2, 20, 89, false);
    }

}

void Piano::OverlayKeyPaint::resized() {
    if (this->getParentComponent() != nullptr) setBounds(this->getParentComponent()->getLocalBounds().reduced(1));
    this->offset = 884-(getWidth()/2);
}

bool Piano::OverlayKeyPaint::keyPressed(const juce::KeyPress& k, juce::Component* c) {
    int code = k.getKeyCode();
    int keyCode = KeysAndCorrespondingKeyCodes[code];
    if (keyCode != -1) {
        this->pressedWhiteKeyCodes.insert(code);
        WhiteKeyDown(keyCode);
        repaint();
        return true;
    }
    keyCode = BlackKeysAndCorrespondingKeyCodes[code];
    if (keyCode != -1) {
        this->pressedBlackKeyCodes.insert(code);
        BlackKeyDown(keyCode);
        repaint();
        return true;
    }

    return false;
}

std::queue<int> WhitePressedTemp;
std::queue<int> BlackPressedTemp;

bool Piano::OverlayKeyPaint::keyStateChanged(bool isKeyDown, juce::Component* c) {
    int num = 0;
    for (auto i = pressedWhiteKeyCodes.begin(); i != pressedWhiteKeyCodes.end(); i++) {
        if (!juce::KeyPress::isKeyCurrentlyDown(*i)) {
            WhitePressedTemp.push(*i);num ++;
            WhiteKeyUp(KeysAndCorrespondingKeyCodes[*i]);
        }
    }
    while(!WhitePressedTemp.empty()) {
        pressedWhiteKeyCodes.erase(WhitePressedTemp.front()); WhitePressedTemp.pop();
    }

    for (auto i = pressedBlackKeyCodes.begin(); i != pressedBlackKeyCodes.end(); i++) {
        if (!juce::KeyPress::isKeyCurrentlyDown(*i)) {
            BlackPressedTemp.push(*i);num ++;
            BlackKeyUp(BlackKeysAndCorrespondingKeyCodes[*i]);
        }
    }
    while(!BlackPressedTemp.empty()) {
        pressedBlackKeyCodes.erase(WhitePressedTemp.front()); BlackPressedTemp.pop();
    }

    return false;
};