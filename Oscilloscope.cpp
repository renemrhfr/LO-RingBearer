/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 6 Sep 2023 5:47:25pm
    Author:  Rene Maierhofer

  ==============================================================================
*/

#include "Oscilloscope.h"

Oscilloscope::Oscilloscope() : juce::AudioVisualiserComponent(1){
    setOpaque(false);
    threLo = 0.0f;
    threHi = 0.0f;
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Glass_jpg, BinaryData::Glass_jpgSize);
}

void Oscilloscope::resized() {
  // waveProgress.setBounds(getLocalBounds().withSizeKeepingCentre(10,10));
}

void Oscilloscope::pushBuffer(const juce::AudioBuffer<float> bufferToPush) {
    AudioVisualiserComponent::pushBuffer(bufferToPush);
}

void Oscilloscope::paint(juce::Graphics& g) {
    juce::Path roundedRectangle;
    roundedRectangle.addRoundedRectangle(getLocalBounds().toFloat(), 10.0f);  // adjust corner radius as needed
    // Draw the image with rounded corners
    g.saveState();
    g.reduceClipRegion(roundedRectangle);
    g.drawImage(backgroundImage, getLocalBounds().toFloat());
    g.restoreState();
    // Define gradient as before
    juce::Point<float> start(0, 0);
    juce::Point<float> end(0, getHeight());
    juce::ColourGradient gradient(
            juce::Colour::fromRGB(215,215,226).withAlpha(0.9f),
            start,
            juce::Colour::fromRGB(46,59,78).withAlpha(0.8f),
            end,
            true
    );
    g.setGradientFill(gradient);
    g.fillPath(roundedRectangle);
    juce::AudioVisualiserComponent::setColours(juce::Colours::transparentWhite, juce::Colours::whitesmoke);
    juce::AudioVisualiserComponent::paint(g);
}

void Oscilloscope::paintChannel (juce::Graphics& g, juce::Rectangle<float> area,
                                             const juce::Range<float>* levels, int numLevels, int nextSample)
{
    //juce::AudioVisualiserComponent::paintChannel(g, area, levels, numLevels, nextSample);


    const auto halfUp = area.getHeight() / 2;

    // Draw positive Values
    for (int i = 0; i < numLevels; ++i) {
        auto level = halfUp + (-(levels[i].getEnd()) * area.getHeight());
        double position = static_cast<double>(i) / (numLevels - 1);
        double newPositionX = position * area.getWidth();
        g.drawVerticalLine(static_cast<int>(newPositionX), level, area.getCentre().getY());
    }

    // Draw negative Values
     for (int i = numLevels; --i >= 0;) {
        auto level = area.getCentre().getY() - ((levels[i].getStart()) * area.getHeight());
         double position = static_cast<double>(i) / (numLevels - 1);
         double newPositionX = position * area.getWidth();
         g.drawVerticalLine(static_cast<int>(newPositionX), area.getCentre().getY(), level);
    }
    paintThresholds(g, area);
}

void Oscilloscope::paintThresholds(juce::Graphics& g, juce::Rectangle<float> area) const {
    const auto halfUp = area.getHeight() / 2;
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawHorizontalLine((int) halfUp, 0, area.getWidth());

    g.setColour(juce::Colour::fromRGB(13,26,38));
    g.drawHorizontalLine(static_cast<int>(halfUp - (halfUp * threHi)), 0, area.getWidth());
    g.drawHorizontalLine(static_cast<int>(halfUp + (halfUp * threHi)), 0, area.getWidth());

    g.setColour(juce::Colour::fromRGB(13,26,38));
    g.drawHorizontalLine(static_cast<int>(halfUp - (halfUp * threLo)), 0, area.getWidth());
    g.drawHorizontalLine(static_cast<int>(halfUp + (halfUp * threLo)), 0, area.getWidth());

    g.setColour(juce::Colour::fromRGB(152,11,14).withAlpha(0.4f));

    juce::Rectangle<float> topArea;
    topArea.setBounds(area.getX(), area.getY(),area.getWidth(), area.getHeight());
    topArea.removeFromBottom(area.getHeight() * 0.5f);
    float removeFromTop = halfUp - (halfUp * threHi);
    topArea.removeFromTop(removeFromTop);
    topArea.removeFromBottom(halfUp * threLo);
    g.fillRect(topArea);

    juce::Rectangle<float> bottomArea;
    bottomArea.setBounds(area.getX(), area.getY(),area.getWidth(), area.getHeight());
    bottomArea.removeFromTop(area.getHeight() * 0.5f);
    removeFromTop = halfUp * threLo;
    bottomArea.removeFromTop(removeFromTop);
    bottomArea.removeFromBottom(halfUp - (halfUp * threHi));
    g.fillRect(bottomArea);

    g.setColour(juce::Colours::whitesmoke.withAlpha(0.15f));
    g.drawFittedText("[LO:RingBearer]", getLocalBounds().withBottom(getLocalBounds().getHeight() - 5), juce::Justification::centredBottom, 1);
}
