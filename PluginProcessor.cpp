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
    std::make_unique<juce::AudioParameterFloat>("ThresholdLow", "ThresholdLow", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("ThresholdHigh", "ThresholdHigh", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1), 1.0f),
    std::make_unique<juce::AudioParameterFloat>("Smoothing", "Smoothing", juce::NormalisableRange<float>(0.0f, 10.0f), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", juce::NormalisableRange<float>(-5.0f, 5.0f), 0.0f)
           })
#endif
{
    refreshSmoothing();
    state left_channel_state;
    state right_channel_state;
    states[0] = left_channel_state;
    states[1] = right_channel_state;
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
    threHi = parameters.getParameter("ThresholdHigh")->getValue();
    threLo = parameters.getParameter("ThresholdLow")->getValue();
    mix = parameters.getParameter("Mix")->getValue();
    gain = parameters.getRawParameterValue("Gain")->load();
    // Display a snapshot of the buffer before applying the effect
    oscilloscope.pushBufferSnapshot(buffer);
    juce::AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);
    // Validate Sidechain buffer. If it's empty, or we don't have enough samples yet: do nothing.
    if (!sidechainBuffer.getNumChannels() || buffer.getNumSamples() > sidechainBuffer.getNumSamples()) {
        buffer.applyGain(juce::Decibels::decibelsToGain(gain));
        return;
    }
    int totalChannelsToProcess = juce::jmin(totalNumInputChannels, sidechainBuffer.getNumChannels());
    if (totalChannelsToProcess > 2)
        return;
    for (int channel = 0; channel < totalChannelsToProcess; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        const auto* sideChainChannelData = sidechainBuffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            auto channel_id = static_cast<unsigned long>(channel);
            float inputSample = channelData[sample];
            if (isInThreshold(inputSample)) {
                if (!states[channel_id].previouslyAboveThresh) {
                    states[channel_id].previouslyAboveThresh = true;
                    states[channel_id].smoothedThreshold.setTargetValue(mix);
                }
            } else {
                if (states[channel_id].previouslyAboveThresh) {
                    states[channel_id].previouslyAboveThresh = false;
                    states[channel_id].smoothedThreshold.setTargetValue(0.0f);
                } else {
                    if (states[channel_id].smoothedMix == 0.0f)
                        continue;
                }
            }
            states[channel_id].smoothedMix = states[channel_id].smoothedThreshold.getNextValue();
            float scSample = sideChainChannelData[sample];
            float ringModulatedSignal = juce::jlimit(-1.0f, 1.0f, inputSample * scSample);
            float mixedSignal = mixSamples(inputSample, ringModulatedSignal, channel_id);
            channelData[sample] = mixedSignal;
        }
    }
    buffer.applyGain(juce::Decibels::decibelsToGain(gain));
}

bool RingBearerAudioProcessor::isInThreshold(float sample) const
{
    return (sample < threHi && sample > threLo) || (sample > -threHi && sample < -threLo);
}

float RingBearerAudioProcessor::mixSamples(float originalSample, float processedSample, unsigned long channel) const
{
    return (1.0f - states[channel].smoothedMix) * originalSample + states[channel].smoothedMix * processedSample;
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

void RingBearerAudioProcessor::refreshSmoothing() {
    states[0].smoothedThreshold.reset(getSampleRate(), parameters.getParameter("Smoothing")->getValue() * 0.001);
    states[1].smoothedThreshold.reset(getSampleRate(), parameters.getParameter("Smoothing")->getValue() * 0.001);
    states[0].previouslyAboveThresh = !states[0].previouslyAboveThresh;
    states[1].previouslyAboveThresh = !states[1].previouslyAboveThresh;
}
