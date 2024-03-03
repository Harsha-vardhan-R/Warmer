#include "MenuComponent.h"
#include "Instrument.h"

MenuComponent::MenuComponent() {
    // `this` already implements the `MenuBarModel` so this is going to return a
    // `MenuBarComponent` with the Model already setup.
    menuComponent = std::make_unique<juce::MenuBarComponent>(this);
    // getting the new component to show up.
    addAndMakeVisible(menuComponent.get());
    setLookAndFeel(this);

    resized(); // this is NEEDED for the first time rendering.
}

MenuComponent::~MenuComponent() {
    setLookAndFeel(nullptr);
}

void MenuComponent::paint(juce::Graphics& g) {}

void MenuComponent::resized() {
    //setting the size of our normal component.
    setSize(getParentWidth(), 25);
    // setting the size of the menuComponent inside this component.
    menuComponent.get()->setSize(getParentWidth(), 25);
}

juce::StringArray MenuComponent::getMenuBarNames() {
    return {"Instrument", "View", "Preferences", "Edit"};
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
    } else if (menuName == "View") {
        menu.addItem(15, "Accent Colour");
        menu.addItem(17, "Size+");
        menu.addItem(18, "Size-");
    } else if (menuName == "Preferences") {
        menu.addItem(20, "Refresh MIDI devices");
        menu.addItem(19, "Audio/MIDI preferences");
        menu.addSeparator();
        menu.addItem(21, "About");
    } else if (menuName == "Edit") {
        menu.addItem(22, "undo");
    }

    return menu;
}

void MenuComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex) {
    auto instrument = Instrument::getInstance();
    switch (menuItemID) {
        case 1:
            instrument->Initialize();
            break;
        case 5:
            std::cout << "Exiting Warmer..." << std::endl;
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            break;
        case 19:
            instrument->OpenAudioAndMIDISettings();
            break;
        case 20:
            instrument->refreshMIDIDevices();
            break;
        default:
            break;
    }
}


void MenuComponent::drawMenuBarItem (juce::Graphics& g,
                      int width,
                      int height,
                      int itemIndex,
                      const juce::String& itemText,
                      bool isMouseOverItem,
                      bool isMenuOpen,
                      bool isMouseOverBar,
                      juce::MenuBarComponent& menuBar) {
    juce::Rectangle<int> highLight(1, 1, width-2, height-2);
    if (isMouseOverItem) {
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
        g.setColour(SelectedTabTextColourID);
    } else {
        g.setColour(IdleTabTextColourID);
    }
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
        g.setColour(SeparatorColourID);
        juce::Rectangle<int> sepLine(area.getX()+3, area.getCentreY(), area.getWidth()-6, 1);
        g.fillRect(sepLine);
        return;
    } else if (isHighlighted) {
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
        g.setColour(SelectedTabTextColourID);
    } else {
        g.setColour(IdleTabTextColourID);
    }

    g.drawText(text, aligner, juce::Justification::centredLeft);

}


void MenuComponent::drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                 int buttonW, int buttonH, juce::ComboBox& box) {
    g.setColour(MenuMouseOverColourID);
    g.fillRoundedRectangle(box.getLocalBounds().reduced(2).toFloat(), 3.0f);

    juce::Path arrow;
    float arrowDimen = 10;
    float arrowX = width - 25; // X position of the arrow
    float arrowY = (height - arrowDimen) / 2; // Y position of the arrow

    if (box.isPopupActive()) {
        arrow.addTriangle(arrowX + (arrowDimen * 0.5f) + 1, arrowY,
                          arrowX, arrowY + arrowDimen,
                          arrowX + arrowDimen + 2, arrowY + arrowDimen);
    } else {
        arrow.addTriangle(arrowX, arrowY,
                          arrowX + arrowDimen + 2, arrowY,
                          arrowX + (arrowDimen * 0.5f) + 1, arrowY + arrowDimen);
    }

    g.setColour(ComboBoxArrowColourID);
    g.fillPath(arrow);
}

//================================ Tab styles ===================================
void MenuComponent::drawTabButton(juce::TabBarButton& button,
                                  juce::Graphics& g,
                                  bool isMouseOver,
                                  bool isMouseDown) {

    g.fillAll(MenuBackgroundID);

    if (button.isFrontTab()) {
        g.setColour(SelectTabColourID);
        g.fillRoundedRectangle(button.getLocalBounds().toFloat().reduced(3.0f), 2.0f);
        // for the square look at the junction.
        juce::Rectangle<int> squ(button.getLocalBounds().getX()+10, button.getLocalBounds().getY()+3, button.getWidth()-10, button.getHeight()-6);
        g.fillRect(squ);g.setColour(juce::Colours::grey);
        g.fillRect(button.getWidth()-2, -5, 2, button.getHeight()+10);
    } else if (isMouseOver) {
        g.setColour(MouseOverTabColourID);
        g.fillRoundedRectangle(button.getLocalBounds().toFloat().reduced(3.0f), 2.0f);
    } else {
        g.setColour(IdleTabColourID);
        g.fillRoundedRectangle(button.getLocalBounds().toFloat().reduced(3.0f), 2.0f);
    }

    if (button.isFrontTab()) {
        g.setColour(SelectedTabTextColourID);
    } else {
        g.setColour(IdleTabTextColourID);
    }

    juce::AffineTransform transform = juce::AffineTransform::rotation(-1.57,
                                                                      button.getLocalBounds().getCentreX(),
                                                                      button.getLocalBounds().getCentreY());
    g.addTransform(transform);
    g.drawText(button.getButtonText(), button.getLocalBounds().reduced(-15), juce::Justification::centred);

}

int MenuComponent::getTabButtonBestWidth(juce::TabBarButton &button, int tabDepth) {
    return 80;
}