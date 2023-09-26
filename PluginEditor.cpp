/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpModAudioProcessorEditor::AmpModAudioProcessorEditor (AmpModAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(400, 200);
    
    // Threshold Low
    threLo.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threLo.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    threLoAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdLow", threLo));
    addAndMakeVisible(threLo);
    threLoLabel.setText(juce::String("Thre - Low"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threLoLabel);
    
    
    threHi.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threHi.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    threHiAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdHigh", threHi));
    addAndMakeVisible(threHi);
    threHiLabel.setText(juce::String("Thre High"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threHi);
    addAndMakeVisible(threHiLabel);
    
    threHi.onValueChange = [this]() {
        audioProcessor.oscilloscope.threHi = threHi.getValue();
    };
    
    threLo.onValueChange = [this]() {
        audioProcessor.oscilloscope.threLo = threLo.getValue();
    };
    
    mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Mix", mix));
    addAndMakeVisible(mix);
    mixLabel.setText(juce::String("Mix"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(mix);
    addAndMakeVisible(mixLabel);
    
    audioProcessor.oscilloscope.setColours(juce::Colours::whitesmoke, juce::Colours::black);
    audioProcessor.oscilloscope.setBufferSize(100);
    audioProcessor.oscilloscope.setSamplesPerBlock(300);
    addAndMakeVisible(audioProcessor.oscilloscope);
    
}

AmpModAudioProcessorEditor::~AmpModAudioProcessorEditor()
{
}

//==============================================================================
void AmpModAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    
}

void AmpModAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();
    
    // X, Y, Width, Height
    threLo.setBounds(20, height * 0.6, 100, height * 0.3);
    threLoLabel.setBounds(20, height * 0.8, 100, height * 0.3);
    
    threHi.setBounds(150, height * 0.6, 100, height * 0.3);
    threHiLabel.setBounds(150, height * 0.8, 100, height * 0.3);
    
    mix.setBounds(280, height * 0.6, 100, height * 0.3);
    mixLabel.setBounds(315, height * 0.8, 50, height * 0.3);
    
    audioProcessor.oscilloscope.setBounds(width*0.1, 10, width*0.8, height*0.5);
    
}
