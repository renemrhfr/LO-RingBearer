#include "RingBearerLookAndFeel.h"

void RingBearerLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width,
                                             int height, float sliderPos,
                                             float rotaryStartAngle, float rotaryEndAngle,
                                             juce::Slider &slider) {

    auto radius = juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = x + width * 0.5f;
    auto centreY = y + height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::ColourGradient gradient(juce::Colours::lightgrey, centreX, ry,
                                  juce::Colours::darkgrey, centreX, ry + rw,
                                  false);
    g.setGradientFill(gradient);
    g.fillEllipse (rx, ry, rw, rw);

    // Outline
    juce::ColourGradient gradient2(juce::Colour::fromRGB(221,201,168), centreX, ry,
                                   juce::Colour::fromRGB(21,32,46), centreX, ry + rw,
                                   true);
    g.setGradientFill(gradient2);
    juce::Path outlinePath;
    float outlineWidth = 2.0f;
    outlinePath.addEllipse(rx - outlineWidth, ry - outlineWidth, rw + 2 * outlineWidth, rw + 2 * outlineWidth);
    outlinePath.addEllipse(rx, ry, rw, rw);
    g.fillPath(outlinePath);
    g.setColour((juce::Colour::fromRGB(90, 107, 125)));
    juce::Path greyArc;
    greyArc.addCentredArc (centreX, centreY, radius, radius, 0, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath (greyArc, juce::PathStrokeType (2.0f));


    // Value Indicator
    g.setColour (juce::Colour::fromRGB(196,81,57));
    juce::Path orangeArc;
    orangeArc.addCentredArc (centreX, centreY, radius, radius, 0, rotaryStartAngle, angle, true);
    g.strokePath (orangeArc, juce::PathStrokeType (2.0f));

    // Value Pointer-Line
    g.setColour (juce::Colours::white);
    auto pointerLength = radius * 0.2f;
    auto pointerWidth = radius * 0.1f;
    juce::Path p;
    p.addRectangle (-pointerWidth * 0.5f, -radius, pointerWidth, pointerLength);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    g.fillPath (p);
}
