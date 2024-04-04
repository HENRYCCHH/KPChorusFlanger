/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KPChorusFlangerAudioProcessorEditor::KPChorusFlangerAudioProcessorEditor (KPChorusFlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600,445);
    
    auto& params = processor.getParameters();
    
    
    
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    
    mDryWetSlider.setBounds(10,0, 180,180);
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 65, 20);
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter  = mDryWetSlider.getValue();};
    mDryWetSlider.onDragStart = [dryWetParameter] {dryWetParameter->beginChangeGesture();};
    mDryWetSlider.onDragEnd = [dryWetParameter] {dryWetParameter->endChangeGesture();};
    
    
    

    juce::AudioParameterFloat* depthParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
    
    mDepthSlider.setBounds(210,0, 180,180);
    mDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 65, 20);
    mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end);
    mDepthSlider.setValue(*depthParameter);
    addAndMakeVisible(mDepthSlider);
    
    mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter  = mDepthSlider.getValue();};
    mDepthSlider.onDragStart = [depthParameter] {depthParameter->beginChangeGesture();};
    mDepthSlider.onDragEnd = [depthParameter] {depthParameter->endChangeGesture();};
    
    
    
    
    juce::AudioParameterFloat* rateParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);
    
    mRateSlider.setBounds(410,0, 180,180);
    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 65, 20);
    mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end);
    mRateSlider.setValue(*rateParameter);
    addAndMakeVisible(mRateSlider);
    
    mRateSlider.onValueChange = [this, rateParameter] { *rateParameter  = mRateSlider.getValue();};
    mRateSlider.onDragStart = [rateParameter] {rateParameter->beginChangeGesture();};
    mRateSlider.onDragEnd = [rateParameter] {rateParameter->endChangeGesture();};
    
    
    
    juce::AudioParameterFloat* phaseParameter = (juce::AudioParameterFloat*)params.getUnchecked(3);
    
    mPhaseOffsetSlider.setBounds(10,220, 180,180);
    mPhaseOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 65, 20);
    mPhaseOffsetSlider.setRange(phaseParameter->range.start, phaseParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseParameter);
    addAndMakeVisible(mPhaseOffsetSlider);
    
    mPhaseOffsetSlider.onValueChange = [this, phaseParameter] { *phaseParameter  = mPhaseOffsetSlider.getValue();};
    mPhaseOffsetSlider.onDragStart = [phaseParameter] {phaseParameter->beginChangeGesture();};
    mPhaseOffsetSlider.onDragEnd = [phaseParameter] {phaseParameter->endChangeGesture();};
    
    
    
    
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(4);
    
    mFeedbackSlider.setBounds(410, 220, 180,180);
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 65, 20);
    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    
    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter  = mFeedbackSlider.getValue();};
    mFeedbackSlider.onDragStart = [feedbackParameter] {feedbackParameter->beginChangeGesture();};
    mFeedbackSlider.onDragEnd = [feedbackParameter] {feedbackParameter->endChangeGesture();};
    
    
    
    
    juce::AudioParameterInt* typeParameter = (juce::AudioParameterInt*)params.getUnchecked(5);

    mType.setBounds(240, 280, 120, 40);
    mType.addItem("Chorus",1);
    mType.addItem("Flanger",2);
    addAndMakeVisible(mType);
    
    mType.onChange = [this, typeParameter] {
        typeParameter->beginChangeGesture();
        *typeParameter = mType.getSelectedItemIndex();
        typeParameter->endChangeGesture();
    };
    
    mType.setSelectedItemIndex(*typeParameter);
    
    
}

KPChorusFlangerAudioProcessorEditor::~KPChorusFlangerAudioProcessorEditor()
{
}

//==============================================================================
void KPChorusFlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    //g.drawFittedText ("Dry Wet", getLocalBounds(), juce::Justification::centredLeft, 1);
    //g.drawFittedText ("Depth", getLocalBounds(), juce::Justification::centred, 1);
    //g.drawFittedText ("Rate", getLocalBounds(), juce::Justification::centredRight, 1);
    //g.drawFittedText ("Phase Offset", getLocalBounds(), juce::Justification::bottomLeft, 1);
    //g.drawFittedText ("Feedback", getLocalBounds(), juce::Justification::bottomRight, 1);
    
    g.drawSingleLineText("Dry Wet", 100,200,juce::Justification::centred);
    g.drawSingleLineText("Depth", 300,200,juce::Justification::centred);
    g.drawSingleLineText("Rate", 500,200,juce::Justification::centred);
    g.drawSingleLineText("Phase Offset", 100,420,juce::Justification::centred);
    g.drawSingleLineText("Feedback", 500,420,juce::Justification::centred);
}

void KPChorusFlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
