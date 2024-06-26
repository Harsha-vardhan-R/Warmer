/*
  ==============================================================================

    Wheels.h
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>


/*
 * Modulation and pitch wheels sit here, also contains the transpose buttons.
*/

class Wheels : public juce::Component {
public:

    static void* instancePtr;

    static void* getInstance() {
        if (instancePtr != nullptr) {
            return instancePtr;
        }
        int *ptr = nullptr;
        int a;
        std::cout << "Wheels 'get instance' is being called before creating the function, CRASH!!!" << "\n";
        a = *ptr;
        return nullptr;// compiler complaining.
    }

    Wheels();
    ~Wheels() override;

    static int TRANSPOSE;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override {
        for (int i = 0; i < wheelAreas.size(); ++i) {
            if (wheelAreas[i]->contains(event.getPosition())) {
                // Mouse clicked inside this rectangle, so start dragging
                isDragging = true;
                dragIndex = i;
                break;
            }
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override {
        int i = dragIndex;
        if (isDragging && dragIndex >= 0 && dragIndex < wheelAreas.size()) {
            // Update the position of the dragged rectangle based on mouse drag
            if (dragIndex == 0) {
                this->pitchWheelLevel = std::clamp(1.0f - (((float)event.y-(float)wheelAreas[i]->getY())/(float)(getParentHeight() - 20)), 0.0f, 1.0f);
            } else if (dragIndex == 1) {
                this->modWheelLevel = std::clamp(1.0f - (((float)event.y-(float)wheelAreas[i]->getY())/(float)(getParentHeight() - 20)), 0.0f, 1.0f);
            }
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent& event) override {
        isDragging = false;
        if (dragIndex == 0) {
            this->pitchWheelLevel = 0.5;
        } else if (dragIndex == 2) {
            this->transpose = std::clamp(this->transpose+1, -5, 5);
            TRANSPOSE = this->transpose;
        } else if (dragIndex == 3) {
            this->transpose = std::clamp(this->transpose-1, -5, 5);
            TRANSPOSE = this->transpose;
        }
        dragIndex = -1;
        repaint();
    }

    void setModWheel(float value);
    void setPitchWheel(float value);


    void setOffset(int n) {
        offset = n;
    }


private:
    float modWheelLevel;
    float pitchWheelLevel;
    int transpose;

    bool isDragging;
    int dragIndex;
    juce::Point<int> dragOffset;
    int dragDist;

    int offset;

    juce::OwnedArray<juce::Rectangle<int>> wheelAreas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Wheels)
};
