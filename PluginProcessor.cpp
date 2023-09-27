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
oscilloscope(),
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

AmpModAudioProcessor::~AmpModAudioProcessor()
{
}

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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AmpModAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmpModAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
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

    oscilloscope.pushBuffer(buffer);


    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto sidechainChannels  = getBusBuffer (buffer, true, 1).getNumChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    juce::AudioBuffer<float> sidechainBuffer = getBusBuffer(buffer, true, 1);
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    // Check if sidechainBuffer is valid
    if (!sidechainBuffer.getNumChannels()) {
        return; // or handle this scenario appropriately
    }

// Check for buffer size mismatch
    if (buffer.getNumSamples() > sidechainBuffer.getNumSamples()) {
        return; // or handle this scenario appropriately
    }

    int totalChannelsToProcess = juce::jmin(totalNumInputChannels, sidechainBuffer.getNumChannels());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

   /* float threLo = 0.0f;
    float threHi = 0.4f;
    */
    
    threHi = parameters.getParameter("ThresholdHigh")->getValue();
    threLo = parameters.getParameter("ThresholdLow")->getValue();
    mix = parameters.getParameter("Mix")->getValue();

    for (int channel = 0; channel < totalChannelsToProcess; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* sideChainChannelData = sidechainBuffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            if ((channelData[sample] < threHi && channelData[sample] > threLo) || (channelData[sample] > -threHi && channelData[sample] < -threLo)) {
                //float noise = random.nextFloat() * 2.0f - 1.0f; // Range from -1 to 1
                float scSample = sideChainChannelData[sample];

               // bandpass.IIRFilterBase::setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), 800));
               // bandpass.processSingleSampleRaw(noise);
                
                float mixedSignal = (1.0f - mix) * channelData[sample] + mix *  (juce::jlimit(-1.0f, 1.0f, channelData[sample] * scSample));
                channelData[sample] = mixedSignal;
            }
        }
    }
    // TODO: Setup a second Oscilloscope to dislpay the modulated sample
    //oscilloscope.pushBuffer(buffer);
}

//==============================================================================
bool AmpModAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmpModAudioProcessor::createEditor()
{
    return new AmpModAudioProcessorEditor (*this, parameters  );
}

//==============================================================================
void AmpModAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AmpModAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModAudioProcessor();
}
