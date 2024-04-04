/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class KPChorusFlangerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    KPChorusFlangerAudioProcessorEditor (KPChorusFlangerAudioProcessor&);
    ~KPChorusFlangerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    KPChorusFlangerAudioProcessor& audioProcessor;
    
//    juce::AudioParameterFloat* mDryWetParameter;
//    juce::AudioParameterFloat* mDepthParameter;
//    juce::AudioParameterFloat* mRateParameter;
//    juce::AudioParameterFloat* mPhaseOffsetParameter;
//    juce::AudioParameterFloat* mFeedbackParameter;
//
//    juce::AudioParameterInt* mTypeParameter;
    
    
    
    
    
    juce::Slider mDryWetSlider;
    juce::Slider mDepthSlider;
    juce::Slider mRateSlider;
    juce::Slider mPhaseOffsetSlider;
    juce::Slider mFeedbackSlider;
    
    juce::ComboBox mType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KPChorusFlangerAudioProcessorEditor)
};
