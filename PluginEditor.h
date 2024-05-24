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
class RingBearerAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    RingBearerAudioProcessorEditor (RingBearerAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~RingBearerAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    RingBearerAudioProcessor& audioProcessor;
    juce::Image backgroundImage;
    RingBearerLookAndFeel ringBearerLookAndFeel;
    juce::Slider threLo;
    juce::Label threLoLabel;
    juce::Slider threHi;
    juce::Label threHiLabel;
    juce::Slider mix;
    juce::Label mixLabel;
    juce::Slider smoothing;
    juce::Label smoothingLabel;
    juce::Slider gain;
    juce::Label gainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threLoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threHiAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> smoothingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingBearerAudioProcessorEditor)
};
