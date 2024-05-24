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
    setSize(500, 250);
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

    gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gain.setLookAndFeel(&ringBearerLookAndFeel);
    gain.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Gain", gain));
    gainLabel.setText(juce::String("Volume"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(gain);
    addAndMakeVisible(gainLabel);


    smoothing.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    smoothing.setLookAndFeel(&ringBearerLookAndFeel);
    smoothing.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    smoothing.addListener(this);
    smoothingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Smoothing", smoothing));
    smoothingLabel.setText(juce::String("Smoothing"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(smoothing);
    addAndMakeVisible(smoothingLabel);

    gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gain.setLookAndFeel(&ringBearerLookAndFeel);
    gain.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "Gain", gain));
    gainLabel.setText(juce::String("Volume"), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(gain);
    addAndMakeVisible(gainLabel);

    audioProcessor.oscilloscope.setColours(juce::Colours::whitesmoke, juce::Colours::black);
    audioProcessor.oscilloscope.setBufferSize(400);
    audioProcessor.oscilloscope.setSamplesPerBlock(64);
    audioProcessor.oscilloscope.threHi=static_cast<float>(threHi.getValue());
    audioProcessor.oscilloscope.threLo=static_cast<float>(threLo.getValue());

    addAndMakeVisible(audioProcessor.oscilloscope);
}

AmpModAudioProcessorEditor::~AmpModAudioProcessorEditor()
{

}

//==============================================================================
void AmpModAudioProcessorEditor::paint (juce::Graphics& g)
{





    g.drawImage (backgroundImage, getLocalBounds ().toFloat ());
    juce::Point<float> start(static_cast<float>(getWidth() / 2), 60);       // Top-left corner
    juce::Point<float> end(0, static_cast<float>(getHeight()));  // Bottom-left corner
    juce::ColourGradient gradient(
            juce::Colour::fromRGB(48,58,72).withAlpha(1.0f),
            start,
            juce::Colour::fromRGB(4,21,28).withAlpha(1.0f),
            end,
            true
    );
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    g.setColour(juce::Colour::fromRGB(84, 101, 109));
    g.drawRoundedRectangle(static_cast<float>(getX() + 10), static_cast<float>(getY() + 10), getWidth()*0.96, getHeight()*0.93, 2.0f, 1.5f);

    g.setColour(juce::Colour::fromRGB(56,75,91));
    g.drawLine(340, static_cast<float>(getHeight() * 0.70), 340, static_cast<float>(getHeight() * 0.87), 1);

    g.drawHorizontalLine(getHeight() * 0.67, 20, getWidth() * 0.95f);

}

void AmpModAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderThatChanged)
{
    if (sliderThatChanged == &smoothing)
        audioProcessor.refreshSmoothing();
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
    threLo.setBounds(50, static_cast<int>(height * 0.7), 40, 40 );
    threLoLabel.setBounds(30, static_cast<int>(height * 0.8), 80, 60);

    threHi.setBounds(160, static_cast<int>(height * 0.7), 40, 40);
    threHiLabel.setBounds(140, static_cast<int>(height * 0.8), 80, 60);

    smoothing.setBounds(265, static_cast<int>(height * 0.7), 40, 40);
    smoothingLabel.setBounds(255, static_cast<int>(height * 0.8), 60, 60);

    mix.setBounds(360, static_cast<int>(height * 0.7), 40, 40);
    mixLabel.setBounds(365, static_cast<int>(height * 0.8), 60, 60);

    gain.setBounds(420, static_cast<int>(height * 0.7), 40, 40);
    gainLabel.setBounds(410, static_cast<int>(height * 0.8), 60, 60);
    audioProcessor.oscilloscope.setBounds(static_cast<int>(width * 0.05), 20, static_cast<int>(width * 0.90),
                                          static_cast<int>(height * 0.55));

}
