#pragma once

#include <JuceHeader.h>
#include "ColourPalette.h"

// We do not need to implement the `MenuBarComponent`,
// it is added as a class member.
/*
    This also acts as a LookAndFeel master class that contains the styles for different classes.
*/
class MenuComponent :   public juce::Component,
                        public juce::MenuBarModel,
                        public juce::LookAndFeel_V4
{
public:
    //========================================
    MenuComponent();
    ~MenuComponent() override;
    //========================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    //========================================
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    //========================================
    void drawMenuBarBackground (juce::Graphics& g, int width, int height, bool, juce::MenuBarComponent& menuBar) override {
        g.fillAll (juce::Colour(0xffeee3e7));
    }
    juce::Font getMenuBarFont (juce::MenuBarComponent& menuBar, int itemIndex, const juce::String& itemText) override {
        return juce::Font(12.0f);
    }
    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override {
        juce::Rectangle<int> popupBounds(0, 0, width, height);
        g.fillAll(MenuBackgroundID);
    }

    void drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator,
                                           bool isActive,
                                           bool isHighlighted,
                                           bool isTicked,
                                           bool hasSubMenu,
                                           const juce::String& text,
                                           const juce::String& shortcutKeyText,
                                           const juce::Drawable* icon,
                                           const juce::Colour* textColourToUse) override;
    void drawMenuBarItem (juce::Graphics& g,
                          int width,
                          int height,
                          int itemIndex,
                          const juce::String& itemText,
                          bool isMouseOverItem,
                          bool isMenuOpen,
                          bool isMouseOverBar,
                          juce::MenuBarComponent& menuBar) override;
    void drawComboBox ( juce::Graphics& g,
                        int width,
                        int height,
                        bool isButtonDown,
                        int buttonX,
                        int buttonY,
                        int buttonW,
                        int buttonH,
                        juce::ComboBox& box) override;
//    void drawComboBoxTextWhenNothingSelected(juce::Graphics& g,
//                                             juce::ComboBox& box,
//                                             juce::Label& text) override;

    // ++++++++++++++ TAB BAR STYLES ++++++++++++++++++
    void drawTabButton(juce::TabBarButton& button,
                       juce::Graphics& g,
                       bool isMouseOver,
                       bool isMouseDown) override;
    int getTabButtonBestWidth(juce::TabBarButton& button,
                              int tabDepth) override;


private:

    juce::PopupMenu menu;

    std::unique_ptr<juce::MenuBarComponent> menuComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};
