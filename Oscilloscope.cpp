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
}

void Oscilloscope::resized() {
  // waveProgress.setBounds(getLocalBounds().withSizeKeepingCentre(10,10));
}

void Oscilloscope::paint(juce::Graphics& g) {
   
    AudioVisualiserComponent::paint(g);
   
     //waveProgress.paint(g);
    /*
    for(int i = 0; i < AudioVisualiserComponent::getLevelSize(); ++i) {
        waveProgress.setBounds(getLocalBounds().withSizeKeepingCentre(20+1, 20));
    }*/
    
}

void Oscilloscope::paintChannel (juce::Graphics& g, juce::Rectangle<float> area,
                                             const juce::Range<float>* levels, int numLevels, int nextSample)
{
    const auto halfUp = area.getHeight() / 2;
    
    g.setColour(juce::Colours::whitesmoke);
    g.drawHorizontalLine(halfUp, 0, area.getWidth());
    
    g.setColour(juce::Colours::red);
    
    g.drawHorizontalLine(halfUp - (halfUp * threHi), 0, area.getWidth());
    
    g.setColour(juce::Colours::green);
    
    g.drawHorizontalLine(halfUp - (halfUp * threLo), 0, area.getWidth());
    
    
    g.setColour(juce::Colours::black);
    
    // Draw positive Values
    for (int i = 0; i < numLevels; ++i) {
    
        auto level = halfUp + (-(levels[i].getEnd()) * area.getHeight());
        double position = static_cast<double>(i) / (numLevels - 1);
        double newPositionX = position * area.getWidth();
        g.drawVerticalLine(newPositionX, level, area.getCentre().getY());
        
    }
    
    // Draw negative Values
     for (int i = numLevels; --i >= 0;) {

        auto level = area.getCentre().getY() - ((levels[i].getStart()) * area.getHeight());
         double position = static_cast<double>(i) / (numLevels - 1);
         double newPositionX = position * area.getWidth();
         g.drawVerticalLine(newPositionX, area.getCentre().getY(), level);
         
    }

    /*
    g.setColour(juce::Colours::yellow);
    g.drawVerticalLine(area.getWidth()-1, area.getCentre().getY(), 100);
    g.drawVerticalLine(0, area.getCentre().getY(), 100);
   */
    
}
