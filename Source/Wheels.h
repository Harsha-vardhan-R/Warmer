/*
  ==============================================================================

    Wheels.h
    Created: 15 Feb 2024 7:23:38am
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/*
 * Modulation and pitch wheels sit here also contains the transpose buttons.
*/

class Wheels : public juce::Component {
public:

    Wheels();
    ~Wheels() override;

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
                this->pitchWheelLevel = std::clamp(1.0f - (((float)event.y-(float)wheelAreas[i]->getY())/(float)(getParentHeight() - 40)), 0.0f, 1.0f);
            } else if (dragIndex == 1) {
                this->modWheelLevel = std::clamp(1.0f - (((float)event.y-(float)wheelAreas[i]->getY())/(float)(getParentHeight() - 40)), 0.0f, 1.0f);
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
        } else if (dragIndex == 3) {
            this->transpose = std::clamp(this->transpose-1, -5, 5);
        }
        dragIndex = -1;
        repaint();
    }


private:
    float modWheelLevel;
    float pitchWheelLevel;
    int transpose;

    bool isDragging;
    int dragIndex;
    juce::Point<int> dragOffset;
    int dragDist;

    juce::OwnedArray<juce::Rectangle<int>> wheelAreas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Wheels)
};