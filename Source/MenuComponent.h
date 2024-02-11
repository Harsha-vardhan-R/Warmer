#pragma once

#include <JuceHeader.h>

class MenuComponent :   public juce::MenuBarComponent,
                        public juce::MenuBarModel
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

private:
    std::unique_ptr<juce::MenuBarComponent> menuComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};

