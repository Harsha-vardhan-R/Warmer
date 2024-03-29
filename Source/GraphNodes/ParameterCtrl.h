/*
  ==============================================================================

    ParameterCtrl.h
    Created: 24 Mar 2024 11:51:56pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MyLookAndFeel.h"

//==============================================================================
/*
 * This is for the controlling of the parameter in a given node from the inside.
 *
 * for example if you have a cutoff then you add a ParameterCtrl slider ype so the user can set it's value
 * and look at the currently set value.
 *
 * This will always have fixed length and height making it easy to abstract.
 * the pointer to this will be stored by the respective socket,
 * but it will be the child directly to the GraphNode the socket is in,
 * it will not be visible when the socket is connected to some other node, because it(value from the other node) controls it.
*/

class ParameterCtrl {
public:

    ParameterCtrl() {}

    ~ParameterCtrl() {}

    // getValue() is the abstraction that is called from process in the respective node to return the
    // selected item or a value.
    //
    // and because floating points can have precision errors better to compare with a greater than or less than
    // rather than equals to if you mean to return a integer.
    virtual float getValue() {
        return 0.0;
    };

    // basically like a resize
    virtual void setBoundForCtlr(juce::Rectangle<int> bound) {}

    virtual void paintAgain() {}

    // lock and unlock while reading the values.
    void lock() { mutex.lock(); }

    void unlock() { mutex.unlock(); }

    std::unique_ptr<juce::LookAndFeel_V4> styles;

private:
    // This is to lock this when others are reading or setting it.
    std::mutex mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterCtrl)
};



// these are the presently existing types.

// THE MENU
// basically a list of values from which one can be selected.
// The discrete knob outputs an integer value that can be used to set the selected value.

class MenuListParameterCtrl : public juce::ComboBox,
                              public ParameterCtrl {
public:

    MenuListParameterCtrl() {
//        addListener(this);

        styles.reset(new MyLookAndFeel());
        setLookAndFeel(styles.get());
    }

    ~MenuListParameterCtrl() {}

    // ID's will be from 1 to the number of options in the list.
    void addItemToList(juce::String& name) {
        addItem(name, Index); Index++;
    }

    // we do not need a function to remove this. \\

    int getSelectedItemsIndex() {
        return getSelectedId(); // returns the index of the currently selected id.
    }

    void paintAgain() override {
        repaint();
    }

    // for menu it sends 0.5 less than the menu option ID, to remove precision errors.
    // use less than 1 less than 2 and so on.
    float getValue() override {
        return ((float)(getSelectedId())-0.5);
    }

    void setBoundForCtlr(juce::Rectangle<int> bound) {
        setBounds(bound);
    }

private:

    int Index = 1;

};



// THE SLIDER : used by many parameters, look and use inspired by blender's nodes in the graph editor.

// we will be able to set the value directly, drag the mouse to change the value or use a other node to control this.
// when we are sliding or setting the value directly in the node, we do not record what the user is doing so
// the resolution of changing will be of the buffer size, but connecting to an external node or an UI item will
// give a smooth change in the controls - this is how it is designed to work, NOT a BUG.

// value is always floating.
class NumericSliderParameterCtrl : public ParameterCtrl,
                                   public juce::TextEditor {
public:

    NumericSliderParameterCtrl(float from, float to, float default_) {
        this->from = from;
        this->to = to;
        this->value = default_;

//        setWantsKeyboardFocus(true);

        if (styles.get() != nullptr) {
            MyLookAndFeel* casted = dynamic_cast<MyLookAndFeel*>(styles.get());
            casted->setMinValue(from);
            casted->setMaxValue(to);
        }

        styles.reset(new MyLookAndFeel());
        setLookAndFeel(styles.get());

    }

    void paintAgain() override {
        repaint();
    }

    bool keyPressed(const juce::KeyPress& key) override {
        if (key == juce::KeyPress::returnKey) {
            value = std::clamp(getText().getFloatValue(), from ,to);
            setText(juce::String(value)); // for out of bound condition.
            return true;
        }

        return false;
    }

    float getValue() override {
        return value;
    }

    ~NumericSliderParameterCtrl() override {};

    void setBoundForCtlr(juce::Rectangle<int> bound) {
        setBounds(bound);
    }

//    void mouseDown(const juce::MouseEvent& event) override {
////        if (!wasFocused()) grabKeyboardFocus(this);
//    }

    // on focus loss we just put back the value as a string in the editor,
    // this is when someone enters a value but does not press enter before clicking somewhere else.
//    void focusLost(juce::Component::FocusChangeType cause) override {
//        setText(juce::String(value));
//    }

private:

    float from, to, value;

};