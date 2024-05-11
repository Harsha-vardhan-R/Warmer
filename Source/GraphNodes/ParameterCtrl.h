/*
  ==============================================================================

    ParameterCtrl.h
    Created: 24 Mar 2024 11:51:56pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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


// invisible container that contains other controls.
class ParameterCtrl : public juce::Component,
                      private juce::Slider::Listener,
                      private juce::ComboBox::Listener {
public:

    ParameterCtrl() {
        setVisible(false);
        valueAtomic.store(0.0);
    }

    ~ParameterCtrl() override {};

    // getValue() is the abstraction that is called from process in the respective node to return the
    // selected item or a value.
    //
    // and because floating points can have precision errors better to compare with a greater than or less than
    // rather than equals to if you mean to return a integer.
    float getValue() {
        return valueAtomic.load();
    };

    int getType() {
        return parameterType;
    }

    void createNewMenu() {
        if (parameterType != -1) {
            std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
            return;
        }

        menuList.reset(new juce::ComboBox);
        menuList.get()->addListener(this);
        valueAtomic.store(0.0);
        parameterType = 1;
    }

    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override {
        valueAtomic.store(comboBoxThatHasChanged->getSelectedId());
    }

    void addItemToList(juce::String name) {
        if (menuList.get()) {
            menuList.get()->addItem(name, index);
            index++;
        }
    }

    void createTextEditor(float from , float to , float val) {
        if (parameterType != -1) {
            std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
            return;
        }

        sliderFloat.reset(new juce::Slider());
        sliderFloat.get()->setRange(from, to, 0.001);
        sliderFloat.get()->setValue(val);
//        textEditor.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
        sliderFloat.get()->setSliderStyle(juce::Slider::LinearBar);
        sliderFloat.get()->addListener(this);
        sliderFloat.get()->setNumDecimalPlacesToDisplay(2);

        this->from = from;
        this->to = to;
        this->value = val;

        parameterType = 2;
    }


    // called from the socket when ready to make it visible.
    void update() {
        // checking if it is not nullptr.
        if (menuList.get()) {
            menuList.get()->setBounds(getLocalBounds().reduced(4));
            addAndMakeVisible(menuList.get());
            // do not ask me if you did not even add one option, dude.
            menuList.get()->setSelectedId(1, juce::sendNotificationSync);
        } else if (sliderFloat.get()) {
            sliderFloat.get()->setBounds(getLocalBounds().reduced(4));
            addAndMakeVisible(sliderFloat.get());
            sliderFloat.get()->setValue(value);
        }
    }

    void sliderValueChanged(juce::Slider* slider) override {
        float value = sliderFloat.get()->getValue();
        valueAtomic.store(value);
    }

    // lock and unlock while reading the values.
    void lock() { mutex.lock(); }

    void unlock() { mutex.unlock(); }

    void paint(juce::Graphics& g) override {}
    void resized() override {}

    int parameterType = -1; // means nothing.
    // ^ 1 for menu and two for the text input.


private:

    // set from the callbacks and read from get value.
    std::atomic<float> valueAtomic;
    // This is to lock this when others are reading or setting it.
    std::mutex mutex;



    // THE MENU
    // basically a list of values from which one can be selected.
    // The discrete knob outputs an integer value that can be used to set the selected value.
    std::unique_ptr<juce::ComboBox> menuList;
    int index = 1;


    // THE SLIDER : used by many parameters, look and use inspired by blender's nodes in the graph editor.
    // we will be able to set the value directly, drag the mouse to change the value or use a other node to control this.
    // when we are sliding or setting the value directly in the node, we do not record what the user is doing so
    // the resolution of changing will be of the buffer size, but connecting to an external node or an UI item will
    // give a smooth change in the controls - this is how it is designed to work, NOT a BUG.
    // value is always floating.
    std::unique_ptr<juce::Slider> sliderFloat;
    float from, to, value;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterCtrl)
};

