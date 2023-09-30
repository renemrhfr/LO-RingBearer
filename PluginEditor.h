/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "BinaryData.h"
#include "RingBearerLookAndFeel.h"

//==============================================================================
/**
*/
class AmpModAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    AmpModAudioProcessorEditor (AmpModAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~AmpModAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    AmpModAudioProcessor& audioProcessor;
    juce::Image backgroundImage;
    RingBearerLookAndFeel ringBearerLookAndFeel;
    juce::Slider threLo;
    juce::Label threLoLabel;
    juce::Slider threHi;
    juce::Label threHiLabel;
    juce::Slider mix;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threLoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threHiAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModAudioProcessorEditor)
};
