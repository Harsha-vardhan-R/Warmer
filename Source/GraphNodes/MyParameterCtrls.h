//
// Created by harshavardhan on 3/6/24.
//

#pragma once
#include <JuceHeader.h>

class envParamCtrl : public juce::Component {
public:

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void envParamCtrltListenerTriggered() = 0;
    };

    envParamCtrl(Listener* parent) {
        // Initialize with first and last points
        points.push_back({ 0.00f, 0.0f });
        points.push_back({ 1.0f, 1.0f });

        controlLevels.push_back(0.5f);
        controlLevels.push_back(0.5f);

        addListener(parent);
    }



    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::white);

        g.setColour(juce::Colours::lightgrey);
        // draw the guide-lines.
        for (int i = 1; i < 4; ++i) {
            g.drawRect(juce::Rectangle<int>(0, (getHeight()/4)*i, getWidth(), 1));
        }

        for (int i = 1; i < 4; ++i) {
            g.drawRect(juce::Rectangle<int>((getWidth()/4)*i, 0, 1, getHeight()));
        }

        juce::Path path;
        path.startNewSubPath(getPointPosition(points[0]));

        for (size_t i = 1; i < points.size(); ++i) {
            // Calculate the control point's y-coordinate
            float yControlPoint = ( (points[i].y - points[i - 1].y) * controlLevels[i - 1] ) + points[i - 1].y;
            // Calculate the control point's x-coordinate to be in the middle

            float controlEffectX = 1.0f - controlLevels[i - 1];

            auto controlPoint =
                    getPointPosition({ ((points[i].x - points[i - 1].x) * controlEffectX) + points[i - 1].x, yControlPoint });

            auto endPoint = getPointPosition(points[i]);
            path.quadraticTo(controlPoint, endPoint);
        }

        g.setColour(juce::Colours::cornflowerblue);
        g.strokePath(path, juce::PathStrokeType(1.5f));

        g.setColour(juce::Colours::darkviolet);
        for (const auto& point : points) {
            auto pointPos = getPointPosition(point);
            g.fillEllipse(pointPos.x - 3, pointPos.y - 3, 6, 6);
        }

        g.setColour(juce::Colours::grey);
        g.drawRect(getLocalBounds());
    }

    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isRightButtonDown()) {
            removePoint(event.position);
        } else {
            if (!SelectPoint(event.position)) {
                // Determine which curve segment to adjust
                for (int i = 1; i < points.size(); ++i) {
                    if (event.position.x < getPointPosition(points[i]).x) {
                        draggingAreaIndex = i - 1;
                        break;
                    }
                }
            }
        }
        lastMousePosition = event.position;
    }

    void mouseDoubleClick(const juce::MouseEvent& event) override {
        if (event.mods.isLeftButtonDown()) {
            auto normalizedPosition = juce::Point<float>(event.position.x / getWidth(), event.position.y / getHeight());

            for (size_t i = 0; i < points.size(); ++i) {
                auto pointPos = getPointPosition(points[i]);

                // Check if the click is near an existing point
                if (pointPos.getDistanceFrom(event.position) < 12.0f) {
                    draggingPointIndex = static_cast<int>(i);
                    return;
                }
            }

            if (normalizedPosition.x > 0.0f && normalizedPosition.x < 1.0f) {
                // Maximum of 10 created points
                if (points.size() > 11) return;

                // Find the index at which to insert the new point
                int pos = -1;

                for (int i = 0; i < points.size(); ++i) {
                    if (normalizedPosition.x < points[i].x) {
                        pos = i;
                        points.insert(points.begin() + i, { normalizedPosition.x , normalizedPosition.y });
                        break;
                    }
                }

                // Insert the corresponding control level
                if (pos != -1 && pos < controlLevels.size()) {
                    controlLevels.insert(controlLevels.begin() + pos, 0.5f);
                }

                draggingPointIndex = pos;
                repaint();
            }
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override {
        if (draggingPointIndex != -1) {
            movePoint(event.position);
        } else if (draggingAreaIndex != -1) {
            adjustCurve(event.position);
        }
        lastMousePosition = event.position;
    }

    void mouseUp(const juce::MouseEvent&) override {
        draggingPointIndex = -1;
        draggingAreaIndex = -1;
        triggerParentSocket();
    }


    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }


private:

    void triggerParentSocket() { listeners.call([](Listener& l) { l.envParamCtrltListenerTriggered(); }); }

    juce::ListenerList<Listener> listeners;


    struct Point { float x, y; };

    std::vector<Point> points;
    // Stores how much curve is in the path between two points
    std::vector<float> controlLevels;

    int draggingPointIndex = -1;
    int draggingAreaIndex = -1;

    juce::Point<float> lastMousePosition = juce::Point<float>(0, 0);

    // Relative to the actual component size
    juce::Point<float> getPointPosition(const Point& point) {
        return { point.x * getWidth(), point.y * getHeight() };
    }

    bool SelectPoint(juce::Point<float> position) {
        for (size_t i = 0; i < points.size(); ++i) {
            auto pointPos = getPointPosition(points[i]);

            if (pointPos.getDistanceFrom(position) < 10.0f) {
                draggingPointIndex = static_cast<int>(i);
                return true;
            }
        }

        // If no point is found, return false
        return false;
    }

    void movePoint(juce::Point<float> position) {
        auto normalizedPosition = juce::Point<float>(position.x / getWidth(), position.y / getHeight());

        if (draggingPointIndex == 0 || draggingPointIndex == points.size() - 1) {
            // Move first or last point vertically only
            points[draggingPointIndex].y = juce::jlimit(0.0f, 1.0f, normalizedPosition.y);
        } else if (draggingPointIndex > 0 && draggingPointIndex < points.size() - 1) {
            // Move other points within bounds
            points[draggingPointIndex].x = std::clamp( normalizedPosition.x, points[draggingPointIndex - 1].x, points[draggingPointIndex + 1].x);
            points[draggingPointIndex].y = std::clamp(normalizedPosition.y, 0.0f, 1.0f);
        }

        repaint();
    }

    void adjustCurve(juce::Point<float> position) {
        // Adjust curve sensitivity
        if (draggingAreaIndex != -1) {
            float sensitivityFactor = 0.004f;
            if (points[draggingAreaIndex].y > points[draggingAreaIndex+1].y) sensitivityFactor *= -1.0f;
            controlLevels[draggingAreaIndex] = std::clamp<float>(controlLevels[draggingAreaIndex] + sensitivityFactor * (position.y - lastMousePosition.y), 0.0, 1.0);
            repaint();
        }
    }

    void removePoint(juce::Point<float> position) {

        for (size_t i = 0; i < points.size(); ++i) {
            auto pointPos = getPointPosition(points[i]);
            if (pointPos.getDistanceFrom(position) < 12.0f && i != 0 && i != points.size() - 1 && (points.size() > 2)) {

                points.erase(points.begin() + i);
                controlLevels.erase(controlLevels.begin() + std::max(0, static_cast<int>(i) - 1));

                repaint();
                return;
            }
        }

        // If no point is found, set the control level to act like a linear growth
        for (int i = 1; i < points.size(); ++i) {
            if (position.x < getPointPosition(points[i]).x) {
                controlLevels[i - 1] = 0.5;
                repaint();
                break;
            }
        }
    }
};
