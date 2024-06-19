#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RingBearerAudioProcessor::RingBearerAudioProcessor()
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
    std::make_unique<juce::AudioParameterFloat>("Low", "Low", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("High", "High", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Intensity", "Intensity", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Dry Gain", "Dry Gain", juce::NormalisableRange<float>(-10.0f, 10.0f), 0.0f)
           })
#endif
{
}

RingBearerAudioProcessor::~RingBearerAudioProcessor() = default;

const juce::String RingBearerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RingBearerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RingBearerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RingBearerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RingBearerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RingBearerAudioProcessor::getNumPrograms()
{
    return 1;
}

int RingBearerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RingBearerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RingBearerAudioProcessor::getProgramName (int index)
{
    return {};
}

void RingBearerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void RingBearerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void RingBearerAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RingBearerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RingBearerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    threHi = parameters.getParameter("High")->getValue();
    threLo = parameters.getParameter("Low")->getValue();
    intensity = parameters.getParameter("Intensity")->getValue();
    dryGain = parameters.getRawParameterValue("Dry Gain")->load();
    // Display a snapshot of the buffer before applying the effect
    oscilloscope.pushBufferSnapshot(buffer);
    juce::AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);
    // Validate Sidechain buffer. If it's empty, or we don't have enough samples yet: do nothing.
    if (!sidechainBuffer.getNumChannels() || buffer.getNumSamples() > sidechainBuffer.getNumSamples()) {
        buffer.applyGain(juce::Decibels::decibelsToGain(dryGain));
        return;
    }
    int totalChannelsToProcess = juce::jmin(totalNumInputChannels, sidechainBuffer.getNumChannels());
    if (totalChannelsToProcess > 2)
        return;
    for (int channel = 0; channel < totalChannelsToProcess; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        const auto* sideChainChannelData = sidechainBuffer.getWritePointer(channel);
        if (sideChainChannelData == nullptr)
            return;
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            auto channel_id = static_cast<unsigned long>(channel);
            float inputSample = channelData[sample];
            if (isInThreshold(inputSample)) {
                float scSample = sideChainChannelData[sample];
                float ringModulatedSignal = juce::jlimit(-1.0f, 1.0f, inputSample * scSample);
                float mixedSignal = mixSamples(inputSample, ringModulatedSignal, channel_id);
                channelData[sample] = mixedSignal;
            } else {
                channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(dryGain);
            }
        }
    }
}

bool RingBearerAudioProcessor::isInThreshold(float sample) const
{
    return (sample < threHi && sample > threLo) || (sample > -threHi && sample < -threLo);
}

float RingBearerAudioProcessor::mixSamples(float originalSample, float processedSample, unsigned long channel) const
{
    return (1.0f - intensity) * originalSample + intensity * processedSample;
}


bool RingBearerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* RingBearerAudioProcessor::createEditor()
{
    return new RingBearerAudioProcessorEditor (*this, parameters  );
}

void RingBearerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void RingBearerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (parameters.state.getType()))
    {
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingBearerAudioProcessor();
}