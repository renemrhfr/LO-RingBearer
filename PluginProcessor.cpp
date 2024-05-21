/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpModAudioProcessor::AmpModAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                  .withInput("Sidechain Input", juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), false)
#endif
                  ),
parameters(*this, nullptr, juce::Identifier ("Lo-RingBearer"),
           {
    // Start, End, Interval, Skew
    std::make_unique<juce::AudioParameterFloat>("ThresholdLow", "ThresholdLow", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("ThresholdHigh", "ThresholdHigh", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", 0.0f, 1.0f, 1.0f),
           })
#endif
{

}

AmpModAudioProcessor::~AmpModAudioProcessor() = default;

//==============================================================================
const juce::String AmpModAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmpModAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AmpModAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AmpModAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AmpModAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmpModAudioProcessor::getNumPrograms()
{
    return 1;
}

int AmpModAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmpModAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmpModAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmpModAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmpModAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void AmpModAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmpModAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void AmpModAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // Clear any output channels that exceed the input channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Display a snapshot of the buffer before applying the effect
    oscilloscope.pushBufferSnapshot(buffer);
    juce::AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);
    // Validate Sidechain buffer. If it's empty, or we don't have enought samples yet: do nothing.
    if (!sidechainBuffer.getNumChannels() || buffer.getNumSamples() > sidechainBuffer.getNumSamples())
        return;
    // Determine number of channels to process
    int totalChannelsToProcess = juce::jmin(totalNumInputChannels, sidechainBuffer.getNumChannels());
    // Retrieve parameter values
    threHi = parameters.getParameter("ThresholdHigh")->getValue();
    threLo = parameters.getParameter("ThresholdLow")->getValue();
    mix = parameters.getParameter("Mix")->getValue();
    gain = parameters.getParameter("Gain")->getValue();
    // Process the buffer
    for (int channel = 0; channel < totalChannelsToProcess; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        const auto* sideChainChannelData = sidechainBuffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float inputSample = channelData[sample];
            if (isInThreshold(inputSample)) {
                // TODO: Slope / Smoothing. Damit, die Obertöne nicht bei z.B. sauberer Snare unerwünscht entstehen.
                float scSample = sideChainChannelData[sample];
                float ringModulatedSignal = juce::jlimit(-1.0f, 1.0f, inputSample * scSample);
                float mixedSignal = mixSamples(inputSample, ringModulatedSignal);
                channelData[sample] = mixedSignal;
            }
        }
    }
    // TODO: Fix Gain Range. 0.5 is effectively reducing the volume, once a sidechain signal comes in!
    buffer.applyGain(gain);
}

bool AmpModAudioProcessor::isInThreshold(float sample) const
{
    return (sample < threHi && sample > threLo) || (sample > -threHi && sample < -threLo);
}

float AmpModAudioProcessor::mixSamples(float originalSample, float processedSample) const
{
    return (1.0f - mix) * originalSample + mix * processedSample;
}


//==============================================================================
bool AmpModAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AmpModAudioProcessor::createEditor()
{
    return new AmpModAudioProcessorEditor (*this, parameters  );
}

//==============================================================================
void AmpModAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AmpModAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (parameters.state.getType()))
    {
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModAudioProcessor();
}
