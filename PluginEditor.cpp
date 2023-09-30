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
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Background_jpeg, BinaryData::Background_jpegSize);

    threLo.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threLo.setLookAndFeel(&ringBearerLookAndFeel);
    threLo.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    threLoAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdLow", threLo));
    threLoLabel.setText(juce::String("Threshold Low"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threLo);
    addAndMakeVisible(threLoLabel);
    threLo.addListener(this);

    threHi.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threHi.setLookAndFeel(&ringBearerLookAndFeel);
    threHi.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    threHiAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "ThresholdHigh", threHi));
    threHiLabel.setText(juce::String("Threshold High"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(threHi);
    addAndMakeVisible(threHiLabel);
    threHi.addListener(this);
    
    mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix.setLookAndFeel(&ringBearerLookAndFeel);
    mix.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Mix", mix));
    mixLabel.setText(juce::String("Mix"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(mix);
    addAndMakeVisible(mixLabel);
    
    audioProcessor.oscilloscope.setColours(juce::Colours::whitesmoke, juce::Colours::black);
    audioProcessor.oscilloscope.setBufferSize(400);
    audioProcessor.oscilloscope.setSamplesPerBlock(64);
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
    juce::Point<float> start(static_cast<float>(getWidth() / 2), 60);       // Top-left corner
    juce::Point<float> end(0, static_cast<float>(getHeight()));  // Bottom-left corner
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
        else
        audioProcessor.oscilloscope.threHi = static_cast<float>(threHi.getValue());
    }
    else if (sliderThatChanged == &threLo)
    {
        if (threLo.getValue() > threHi.getValue())
            threLo.setValue(threHi.getValue(), juce::NotificationType::dontSendNotification);
        else
        audioProcessor.oscilloscope.threLo = static_cast<float>(threLo.getValue());
    }
}

void AmpModAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();
    
    // X, Y, Width, Height
    threLo.setBounds(20, static_cast<int>(height * 0.7), 80, static_cast<int>(height * 0.2));
    threLoLabel.setBounds(20, static_cast<int>(height * 0.85), 80, static_cast<int>(height * 0.2));
    
    threHi.setBounds(150, static_cast<int>(height * 0.7), 80, static_cast<int>(height * 0.2));
    threHiLabel.setBounds(150, static_cast<int>(height * 0.85), 80, static_cast<int>(height * 0.2));
    
    mix.setBounds(280, static_cast<int>(height * 0.7), 100, static_cast<int>(height * 0.2));
    mixLabel.setBounds(315, static_cast<int>(height * 0.85), 50, static_cast<int>(height * 0.2));
    
    audioProcessor.oscilloscope.setBounds(static_cast<int>(width * 0.03), 10, static_cast<int>(width * 0.95),
                                          static_cast<int>(height * 0.6));
}
