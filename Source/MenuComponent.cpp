#include "MenuComponent.h"

MenuComponent::MenuComponent() {
    // `this` already implements the `MenuBarModel` so this is going to return a
    // `MenuBarComponent` with the Model already setup.
    menuComponent.reset(new juce::MenuBarComponent(this));
    // getting the new component to show up.
    addAndMakeVisible(menuComponent.get());
    setLookAndFeel(this);

    resized(); // this is NEEDED for the first time rendering.
}

MenuComponent::~MenuComponent() {
    setLookAndFeel(nullptr);
}

void MenuComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::lightgrey);
}

void MenuComponent::resized() {
    //setting the size of our normal component.
    setSize(getParentWidth(), 25);
    // setting the size of the menuComponent inside this component.
    menuComponent.get()->setSize(getParentWidth(), 25);
}

juce::StringArray MenuComponent::getMenuBarNames() {
    return {"Instrument", "Settings", "Edit"};
}

juce::PopupMenu MenuComponent::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) {
    juce::PopupMenu menu;

    if (menuName == "Instrument") {
        menu.addItem(1, "Initialize");
        menu.addItem(2, "Load");
        menu.addItem(3, "Save");
        menu.addItem(4, "Save as");
        menu.addItem(6, "Export blueprint");
        menu.addSeparator();
        menu.addItem(5, "Exit");
    } else if (menuName == "Settings") {
        menu.addItem(15, "Theme");
        menu.addItem(16, "Scale+");
        menu.addItem(17, "Scale-");
        menu.addSeparator();
        menu.addItem(18, "MIDI");
        menu.addItem(19, "Audio preferences");
        menu.addItem(20, "About");
    } else if (menuName == "Edit") {
        menu.addItem(21, "undo");
    }

    return menu;
}

void MenuComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex) {
    switch (menuItemID) {
        case 1:

            break;
        case 5:
            std::cout << "Exiting Warmer..." << std::endl;
            break;
        default:
            break;
    }
}

//
void MenuComponent::drawMenuBarItem (juce::Graphics& g,
                      int width,
                      int height,
                      int itemIndex,
                      const juce::String& itemText,
                      bool isMouseOverItem,
                      bool isMenuOpen,
                      bool isMouseOverBar,
                      juce::MenuBarComponent& menuBar) {
    juce::Rectangle<int> highLight(3, 3, width-6, height-6);
    if (isMouseOverItem) {
        g.setColour(juce::Colour(0xff005931));
        g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
    }
    g.setColour(juce::Colours::white);
    g.drawText(itemText, 0, 0, width, height, juce::Justification::centred);
}

void MenuComponent::drawPopupMenuItem (juce::Graphics& g,
                                       const juce::Rectangle<int>& area,
                                       bool isSeparator,
                                       bool isActive,
                                       bool isHighlighted,
                                       bool isTicked,
                                       bool hasSubMenu,
                                       const juce::String& text,
                                       const juce::String& shortcutKeyText,
                                       const juce::Drawable* icon,
                                       const juce::Colour* textColourToUse) {
    juce::Rectangle<int> highLight(area.getX() + 3, area.getY(), area.getWidth() - 6 , area.getHeight());
    juce::Rectangle<int> aligner(area.getX() + 15, area.getY(), area.getWidth() , area.getHeight());
    if (isSeparator) {
        g.setColour(juce::Colours::white);
        g.drawLine(area.getX(), area.getCentreY(), area.getX() + area.getWidth(), area.getCentreY(), 0.1f);
    } else if (isHighlighted) {
        g.setColour(juce::Colour(0xff005931));
        g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
    }
    g.setColour(juce::Colours::white);
    g.drawText(text, aligner, juce::Justification::centredLeft);

}