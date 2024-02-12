#pragma once

#include <JuceHeader.h>

// We do not need to implement the `MenuBarComponent`,
// it is added as a class member.
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
        g.fillAll (juce::Colour(0xff262b2b));
    }
    juce::Font getMenuBarFont (juce::MenuBarComponent& menuBar, int itemIndex, const juce::String& itemText) override {
        return juce::Font(22.0f);
    }
    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override {
        juce::Rectangle<int> popupBounds(0, 0, width, height);
        g.fillAll(juce::Colour(0xff262b2b));
    }
    void drawMenuBarItem (juce::Graphics& g,
                          int width,
                          int height,
                          int itemIndex,
                          const juce::String& itemText,
                          bool isMouseOverItem,
                          bool isMenuOpen,
                          bool isMouseOverBar,
                          juce::MenuBarComponent& menuBar) override;
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



private:
    std::unique_ptr<juce::MenuBarComponent> menuComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};
