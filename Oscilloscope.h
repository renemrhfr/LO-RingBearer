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

class Oscilloscope : public juce::AudioVisualiserComponent {
  
public:
    Oscilloscope();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void paintChannel (juce::Graphics&, juce::Rectangle<float> bounds,
                       const juce::Range<float>* levels, int numLevels, int nextSample) override;
    
    float threHi;
    float threLo;
    
};
