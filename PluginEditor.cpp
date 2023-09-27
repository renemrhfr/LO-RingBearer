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
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Background_jpeg, BinaryData::Background_jpegSize);
    setSize(400, 200);
    
    // Threshold Low
    threLo.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threLo.setLookAndFeel(&ringBearerLookAndFeel);
    threLo.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    threLoAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdLow", threLo));
    addAndMakeVisible(threLo);
    threLoLabel.setText(juce::String("Threshold Low"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threLoLabel);

    threHi.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threHi.setLookAndFeel(&ringBearerLookAndFeel);
    threHi.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    threHiAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdHigh", threHi));
    addAndMakeVisible(threHi);
    threHiLabel.setText(juce::String("Threshold Low"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threHi);
    addAndMakeVisible(threHiLabel);

    threLo.addListener(this);
    threHi.addListener(this);
    
    threHi.onValueChange = [this]() {
        audioProcessor.oscilloscope.threHi = threHi.getValue();
    };
    
    threLo.onValueChange = [this]() {
        audioProcessor.oscilloscope.threLo = threLo.getValue();
    };
    
    mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix.setLookAndFeel(&ringBearerLookAndFeel);
    mix.setTextBoxStyle(juce::Slider::NoTextBox, 0,0,0);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Mix", mix));
    addAndMakeVisible(mix);
    mixLabel.setText(juce::String("Mix"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(mix);
    addAndMakeVisible(mixLabel);
    
    audioProcessor.oscilloscope.setColours(juce::Colours::whitesmoke, juce::Colours::black);
    audioProcessor.oscilloscope.setBufferSize(1500);
    audioProcessor.oscilloscope.setSamplesPerBlock(25);
    addAndMakeVisible(audioProcessor.oscilloscope);
    
}

AmpModAudioProcessorEditor::~AmpModAudioProcessorEditor()

{
    threLo.removeListener(this);
    threHi.removeListener(this);
}

//==============================================================================
void AmpModAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage (backgroundImage, getLocalBounds ().toFloat ());
    juce::Point<float> start(getWidth()/2, 60);       // Top-left corner
    juce::Point<float> end(0, getHeight());  // Bottom-left corner
    juce::ColourGradient gradient(
            juce::Colour::fromRGB(25,40,54).withAlpha(0.6f),
            start,
            juce::Colour::fromRGB(9,14,19).withAlpha(0.9f),
            end,
            true
    );
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());
}

void AmpModAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderThatChanged)
{
    if (sliderThatChanged == &threHi)
    {
        if (threHi.getValue() < threLo.getValue())
            threHi.setValue(threLo.getValue(), juce::NotificationType::dontSendNotification);
    }
    else if (sliderThatChanged == &threLo)
    {
        if (threLo.getValue() > threHi.getValue())
            threLo.setValue(threHi.getValue(), juce::NotificationType::dontSendNotification);
    }
}

void AmpModAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();
    
    // X, Y, Width, Height
    threLo.setBounds(20, height * 0.7, 80, height * 0.2);
    threLoLabel.setBounds(20, height * 0.85, 80, height * 0.2);
    
    threHi.setBounds(150, height * 0.7, 80, height * 0.2);
    threHiLabel.setBounds(150, height * 0.85, 80, height * 0.2);
    
    mix.setBounds(280, height * 0.7, 100, height * 0.2);
    mixLabel.setBounds(315, height * 0.85, 50, height * 0.2);
    
    audioProcessor.oscilloscope.setBounds(width*0.03, 10, width*0.95, height*0.6);
}
