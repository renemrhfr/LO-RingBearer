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
    std::make_unique<juce::AudioParameterFloat>("ThresholdLow", "ThresholdLow", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("ThresholdHigh", "ThresholdHigh", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.5), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1), 1.0f)
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

void AmpModAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Oscilloscope should display a Snapshot of the Buffer before the Effect was applied
    oscilloscope.pushBufferSnapshot(buffer);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    juce::AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // If SidechainBuffer is not Valid, don't do any processing
    if (!sidechainBuffer.getNumChannels()) {
        return;
    }

    // If we don't have enough Samples from SidechainBuffer yet, don't do any processing
    if (buffer.getNumSamples() > sidechainBuffer.getNumSamples()) {
        return;
    }

    // If we have a Mono Sidechain-Signal, we can only process one channel
    int totalChannelsToProcess = juce::jmin(totalNumInputChannels, sidechainBuffer.getNumChannels());

    threHi = parameters.getParameter("ThresholdHigh")->getValue();
    threLo = parameters.getParameter("ThresholdLow")->getValue();
    mix = parameters.getParameter("Mix")->getValue();

    for (int channel = 0; channel < totalChannelsToProcess; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto const* sideChainChannelData = sidechainBuffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            if ((channelData[sample] < threHi && channelData[sample] > threLo) || (channelData[sample] > -threHi && channelData[sample] < -threLo)) {
                float scSample = sideChainChannelData[sample];
                float mixedSignal = (1.0f - mix) * channelData[sample] + mix *  (juce::jlimit(-1.0f, 1.0f, channelData[sample] * scSample));
                channelData[sample] = mixedSignal;
            }
        }
    }
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
