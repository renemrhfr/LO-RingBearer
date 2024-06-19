/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RingBearerAudioProcessorEditor::RingBearerAudioProcessorEditor (RingBearerAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(500, 250);
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::BG_jpg, BinaryData::BG_jpgSize);

    threLo.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threLo.setLookAndFeel(&ringBearerLookAndFeel);
    threLo.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    threLoAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Low", threLo));
    threLoLabel.setText(juce::String("Low"), juce::NotificationType::dontSendNotification);
    threLoLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(threLo);
    addAndMakeVisible(threLoLabel);
    threLo.addListener(this);

    threHi.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threHi.setLookAndFeel(&ringBearerLookAndFeel);
    threHi.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    threHiAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "High", threHi));
    threHiLabel.setText(juce::String("High"), juce::NotificationType::dontSendNotification);
    threHiLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(threHi);
    addAndMakeVisible(threHiLabel);
    threHi.addListener(this);

    mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix.setLookAndFeel(&ringBearerLookAndFeel);
    mix.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    mix.addListener(this);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Intensity", mix));

    mixLabel.setText(juce::String("Intensity"), juce::NotificationType::dontSendNotification);
    mixLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(mix);
    addAndMakeVisible(mixLabel);

    gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gain.setLookAndFeel(&ringBearerLookAndFeel);
    gain.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Dry Gain", gain));
    gainLabel.setText(juce::String("Dry Level"), juce::NotificationType::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(gain);
    addAndMakeVisible(gainLabel);

    audioProcessor.oscilloscope.setColours(juce::Colours::whitesmoke, juce::Colours::black);
    audioProcessor.oscilloscope.setBufferSize(400);
    audioProcessor.oscilloscope.setSamplesPerBlock(64);
    audioProcessor.oscilloscope.threHi=static_cast<float>(threHi.getValue());
    audioProcessor.oscilloscope.threLo=static_cast<float>(threLo.getValue());

    addAndMakeVisible(audioProcessor.oscilloscope);
}

RingBearerAudioProcessorEditor::~RingBearerAudioProcessorEditor()
{

}

//==============================================================================
void RingBearerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage (backgroundImage, getLocalBounds().toFloat());
    g.setColour(juce::Colour::fromRGB(84, 101, 109));
    g.setColour(juce::Colour::fromRGB(56,75,91));

    g.drawLine(130, static_cast<float>(getHeight() * 0.80), 130, static_cast<float>(getHeight() * 0.95), 1);
    g.drawLine(410, static_cast<float>(getHeight() * 0.80), 410, static_cast<float>(getHeight() * 0.95), 1);
}

void RingBearerAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderThatChanged)
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

void RingBearerAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();

    threLoLabel.setBounds(160, static_cast<int>(height * 0.72), 100, 40);
    threLo.setBounds(160, static_cast<int>(height * 0.84), 35, 35 );

    threHiLabel.setBounds(237, static_cast<int>(height * 0.72), 100, 40);
    threHi.setBounds(238, static_cast<int>(height * 0.84), 35, 35);

    mixLabel.setBounds(309, static_cast<int>(height * 0.72), 100, 40);
    mix.setBounds(318, static_cast<int>(height * 0.84), 40, 35);

    gainLabel.setBounds(425, static_cast<int>(height * 0.72), 100, 40);
    gain.setBounds(440, static_cast<int>(height * 0.84), 40, 35);

    audioProcessor.oscilloscope.setBounds(0, 0, width,
                                          static_cast<int>(height * 0.75));

}
