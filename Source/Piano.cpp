/*
  ==============================================================================

    Piano.cpp
    Created: 15 Feb 2024 7:23:13am
    Author:  harshavardhan

  ==============================================================================
*/

#include "Piano.h"

int Wheels::TRANSPOSE = 0;

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

// Bad naming, Sorry :(
int KeysAndCorrespondingKeyCodes[150];
int KeyMap[12] = {97, 115, 100, 102, 103, 104, 106};
int PianoKeyMap[12] = {21, 22, 23, 24, 25, 26, 27};
int BlackKeysAndCorrespondingKeyCodes[150];
int BlackKeyMap[12] = {119, 101, 116, 121, 117};
int BlackPianoKeyMap[12] = {15, 16, 17, 18, 19};



Piano::Piano() {
    overlayPainter.reset(new Piano::OverlayKeyPaint());
    overlayPainter.get()->setWantsKeyboardFocus(true);
    overlayPainter.get()->addKeyListener(overlayPainter.get());
    addAndMakeVisible(overlayPainter.get());

    setBufferedToImage(true);

    resized();
}

Piano::~Piano() {}

void Piano::paint(juce::Graphics &g) {
    int offset = 885-(getWidth()/2);
    g.drawImage(PianoImage, 0 , -3, getWidth(), getHeight(),
                offset, 0, getWidth(), getHeight(), false);

//    g.setColour(juce::Colours::grey);
//    g.drawRect(getLocalBounds(), 2);
}

void Piano::resized() {
    setBounds(150, 0 ,getParentWidth()-150, getParentHeight());

    if (overlayPainter.get() != nullptr) {
        overlayPainter.get()->resized();
    }

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

    setWantsKeyboardFocus(true);

    resized();

}



int presentKey;

void Piano::OverlayKeyPaint::WhiteKeyDown(int keyIndex) {
    presentKey = keyIndex+(Wheels::TRANSPOSE*7);
    if (presentKey < 0 || presentKey > 52) return;
    this->pressedWhiteKeys.insert(presentKey);
}

void Piano::OverlayKeyPaint::WhiteKeyUp(int keyIndex) {
    // does not raise an exception if not found, so no need to handle any kind of exception.
    this->pressedWhiteKeys.erase(keyIndex+(Wheels::TRANSPOSE*7));
}

void Piano::OverlayKeyPaint::BlackKeyDown(int keyIndex) {
    presentKey = keyIndex+(Wheels::TRANSPOSE*5);
    if (presentKey < 0 || presentKey > 36) return;
    this->pressedBlackKeys.insert(presentKey);
}

void Piano::OverlayKeyPaint::BlackKeyUp(int keyIndex) {
    this->pressedBlackKeys.erase(keyIndex+(Wheels::TRANSPOSE*5));
}




void Piano::OverlayKeyPaint::paint(juce::Graphics &g) {

    // Draw the overlay for the pressed white keys.
    for (auto i = pressedWhiteKeys.begin(); i != pressedWhiteKeys.end(); ++i) {
        g.drawImage(WImage, WhiteKeyPositions[*i] - offset - 1, -3, 34, 130,
                    WhiteKeyPositions[*i], 1, 34, 130, false);
    }

    for (auto i = pressedBlackKeys.begin(); i != pressedBlackKeys.end(); ++i) {
        g.drawImage(Bimage, blackKeyPianoPosition[*i] - offset, 3, 20, 85,
                    0, 6, 20, 85, false);
    }

}

void Piano::OverlayKeyPaint::resized() {
    if (this->getParentComponent() != nullptr) setBounds(this->getParentComponent()->getLocalBounds().reduced(1));
    this->offset = 884-(getWidth()/2);
}

bool Piano::OverlayKeyPaint::keyPressed(const juce::KeyPress& k, juce::Component* c) {

    int code = k.getKeyCode();
    int keyCode = KeysAndCorrespondingKeyCodes[code];

    if (keyCode != -1 && (this->pressedWhiteKeyCodes.find(code) == this->pressedWhiteKeyCodes.end())) {
        this->pressedWhiteKeyCodes.insert(code);
        WhiteKeyDown(keyCode);
        repaint();
        return true;
    }

    if (BlackKeysAndCorrespondingKeyCodes[code] != -1 && (this->pressedBlackKeyCodes.find(code) == this->pressedBlackKeyCodes.end())) {
        this->pressedBlackKeyCodes.insert(code);
        BlackKeyDown(BlackKeysAndCorrespondingKeyCodes[code]);
        repaint();
        return true;
    }

    return false;
}




std::queue<int> WhitePressedTemp;
std::queue<int> BlackPressedTemp;

bool Piano::OverlayKeyPaint::keyStateChanged(bool isKeyDown, juce::Component* c) {

    bool stateChange = false;

    for (auto i = pressedWhiteKeyCodes.begin(); i != pressedWhiteKeyCodes.end(); i++) {
        if (!juce::KeyPress::isKeyCurrentlyDown(*i)) {
            WhitePressedTemp.push(*i);
            WhiteKeyUp(KeysAndCorrespondingKeyCodes[*i]);
        }
    }
    while(!WhitePressedTemp.empty()) {
        pressedWhiteKeyCodes.erase(WhitePressedTemp.front()); WhitePressedTemp.pop();stateChange = true;
    }

    for (auto i = pressedBlackKeyCodes.begin(); i != pressedBlackKeyCodes.end(); i++) {
        if (!juce::KeyPress::isKeyCurrentlyDown(*i)) {
            BlackPressedTemp.push(*i);
            BlackKeyUp(BlackKeysAndCorrespondingKeyCodes[*i]);
        }
    }
    while(!BlackPressedTemp.empty()) {
        pressedBlackKeyCodes.erase(BlackPressedTemp.front()); BlackPressedTemp.pop();stateChange = true;
    }

    if (stateChange) repaint();

    return false;
};