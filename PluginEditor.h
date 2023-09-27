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
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    juce::Image backgroundImage;
    RingBearerLookAndFeel ringBearerLookAndFeel;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AmpModAudioProcessor& audioProcessor;
    juce::Slider threHi;
    juce::Slider threLo;
    juce::Slider mix;
    
    juce::Label threLoLabel;
    juce::Label threHiLabel;
    juce::Label mixLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threLoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threHiAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModAudioProcessorEditor)
};
