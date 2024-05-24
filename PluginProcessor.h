/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Oscilloscope.h"

//==============================================================================
/**
*/
class AmpModAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    AmpModAudioProcessor();
    ~AmpModAudioProcessor() override;
    
    juce::Random random;
    
    float threLo;
    float threHi;
    float mix;
    float gain;

    Oscilloscope oscilloscope;
    
    juce::AudioProcessorValueTreeState parameters; //

    struct state {
        float smoothedMix;
        bool previouslyAboveThresh;
        juce::SmoothedValue<float> smoothedThreshold;
    };

    std::array<state, 1> states;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isInThreshold(float sample) const;
    void refreshSmoothing();
    void refreshMix();
    float mixSamples(float originalSample, float processedSample, int channel) const;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModAudioProcessor)
};
