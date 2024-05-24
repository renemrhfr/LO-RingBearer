#ifndef LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H
#define LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H

#include "JuceHeader.h"

class RingBearerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;
};

#endif //LO_RINGBEARER_RINGBEARERLOOKANDFEEL_H
