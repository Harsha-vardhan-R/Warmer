#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class WarmerApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    WarmerApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override { /* DO NOTHING */ }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow (juce::String name)
                : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
                                  DocumentWindow::allButtons) {

            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

            #if JUCE_IOS || JUCE_ANDROID
                setFullScreen (true);
            #else
                setResizable (true, true);
                centreWithSize (getWidth(), getHeight());
            #endif

            this->setConstrainer(getConstrainer());

            setVisible (true);
            setResizable(true, true);
        }

        // Setting the minimum height and width that the app can be resized to.
        juce::ComponentBoundsConstrainer* getConstrainer() {
            constrain.reset(new juce::ComponentBoundsConstrainer());
            constrain.get()->setMinimumWidth(1050);
            constrain.get()->setMinimumHeight(700);

            constrain.get()->setMaximumWidth(1922);
            return constrain.get();
        }

        void closeButtonPressed() override {

            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        std::unique_ptr<juce::ComponentBoundsConstrainer> constrain;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (WarmerApplication)