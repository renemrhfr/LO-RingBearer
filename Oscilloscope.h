/*
  ==============================================================================

    Oscilloscope.h
    Created: 6 Sep 2023 5:47:25pm
    Author:  Rene Maierhofer

  ==============================================================================
*/

#pragma once

#include <stdio.h>
#include "JuceHeader.h"
#include "BinaryData.h"

class Oscilloscope : public juce::AudioVisualiserComponent {
  
public:
    float threHi = 1.0f;
    float threLo = 0.0f;
    juce::Image backgroundImage;
    Oscilloscope();
    void paint (juce::Graphics& g) override;
    void paintThresholds(juce::Graphics& g, juce::Rectangle<float> area) const;
    void paintChannel (juce::Graphics&, juce::Rectangle<float> bounds,
                       const juce::Range<float>* levels, int numLevels, int nextSample) override;
    // Copy instead of Reference to have a "Snapshot" of the Dry Buffer
    void pushBufferSnapshot (const juce::AudioBuffer<float>& bufferToPush);
    
};
