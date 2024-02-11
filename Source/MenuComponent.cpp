#include "MenuComponent.h"

MenuComponent::MenuComponent() {
    menuComponent.reset(new juce::MenuBarComponent(this));
    addAndMakeVisible(menuComponent.get());
    setModel(this);
}

MenuComponent::~MenuComponent() {
    // We do not delete the Component before the model.
    setModel(nullptr);
}

void MenuComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::beige);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
}

void MenuComponent::resized() {
    setSize(getParentWidth(), 20);
    menuComponent.get()->setSize(getParentWidth(), 20);
}

juce::StringArray MenuComponent::getMenuBarNames() {
    return {"Instrument", "Settings", "Edit"};
}

juce::PopupMenu MenuComponent::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) {
    juce::PopupMenu menu;

    if (menuName == "Instrument") {
        menu.addItem(1, "Initialize");
        menu.addSeparator();
        menu.addItem(2, "Load");
        menu.addItem(3, "Save");
        menu.addItem(4, "Save as");
        menu.addSeparator();
        menu.addItem(5, "Exit");
    } else if (menuName == "Settings") {
        menu.addItem(15, "Theme");
        menu.addItem(16, "Scale+");
        menu.addItem(17, "Scale-");
    } else if (menuName == "Edit") {
        menu.addItem(21, "undo");
    }

    return menu;
}

void MenuComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex) {
    switch (menuItemID) {
        case 1:
            std::cout << "Initialize selected" << "\n";
            break;
        case 5:
            std::cout << "Exit selected";
            break;
        default:
            break;
    }
}