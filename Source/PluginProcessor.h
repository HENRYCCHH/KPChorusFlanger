/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define MAX_DELAY_TIME 2
//==============================================================================
/**
*/
class KPChorusFlangerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    KPChorusFlangerAudioProcessor();
    ~KPChorusFlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    
    
    float lin_interp(float inSampleX, float inSampleY, float inFloatPhase);

private:
    
    float mPhaseOffsetSmoothed;
    float mRateSmoothed;
    float mDepthSmoothed;
    
    /** parameter Declarations */
    juce::AudioParameterFloat* mDryWetParameter;
    juce::AudioParameterFloat* mDepthParameter;
    juce::AudioParameterFloat* mRateParameter;
    juce::AudioParameterFloat* mPhaseOffsetParameter;
    juce::AudioParameterFloat* mFeedbackParameter;
    
    juce::AudioParameterInt* mTypeParameter;
    
    
    /** Circular Buffer Data */
    float* mCircularBufferLeft;
    float* mCircularBufferRight;
    
    int mCircularBufferLength;
    int mCircularBufferWriteHead;
    
    float mFeedbackLeft;
    float mFeedbackRight;
    
    float mDelayTimeInSamplesLeft;
    float mDelayTimeInSamplesRight;
    float mDelayReadHeadLeft;
    float mDelayReadHeadRight;
    
    
    /**LFO Data */
    float mLFOPhase;


    
    
    float mDryWet;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KPChorusFlangerAudioProcessor)
};
