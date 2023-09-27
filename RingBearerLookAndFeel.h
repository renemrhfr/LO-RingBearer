//
// Created by Rene Maierhofer on 27.09.23.
//

#ifndef LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H
#define LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H

#include "JuceHeader.h"


class RingBearerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 2.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        if (slider.isEnabled())
            g.setColour (juce::Colour::fromRGB(41,69,82).withAlpha (isMouseOver ? 1.0f : 0.4f));
        else
            g.setColour (juce::Colours::whitesmoke);

        {
            juce::Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, angle, 0.0);
            g.fillPath (filledArc);
        }

        {
            auto lineThickness = juce::jmin (15.0f, (float) juce::jmin (width, height) * 0.45f) * 0.1f;
            juce::Path outlineArc;
            outlineArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, 0.0);
            g.strokePath (outlineArc, juce::PathStrokeType (lineThickness));
        }
    }
};


#endif //LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H
