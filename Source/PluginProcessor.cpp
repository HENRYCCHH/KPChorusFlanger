/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KPChorusFlangerAudioProcessor::KPChorusFlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    /** Construct &  Add Our Parameter*/
    
    addParameter(mDryWetParameter = new juce::AudioParameterFloat("drywet",
                                                                  "Dry Wet",
                                                                  0,
                                                                  1.0,
                                                                  0.5));
    
    addParameter(mDepthParameter = new juce::AudioParameterFloat("depth",
                                                                  "Depth",
                                                                  0,
                                                                  1.0,
                                                                  0.5));
    
    addParameter(mRateParameter = new juce::AudioParameterFloat("rate",
                                                                "Rate",
                                                                  0.1f,
                                                                  20.f,
                                                                  10.f));
    
    addParameter(mPhaseOffsetParameter = new juce::AudioParameterFloat("phaseoffset",
                                                                       "Phase Offset",
                                                                       0.0f,
                                                                       1.f,
                                                                       0.f));
    
    
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat("feedback",
                                                                     "Feedback",
                                                                     0,
                                                                     0.98,
                                                                     0.5));
    
    addParameter(mTypeParameter = new juce::AudioParameterInt("type",
                                                              "Type",
                                                              0,
                                                              1,
                                                              0));
    
    mPhaseOffsetSmoothed = 0;
    mRateSmoothed = 0;
    mDepthSmoothed = 0;
    
    /** initialize our data to default values */
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamplesLeft = 0;
    mDelayTimeInSamplesRight = 0;
    mDelayReadHeadLeft = 0;
    mDelayReadHeadRight = 0;
    
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    
    mDryWet = 0.5;
    
    mLFOPhase = 0;
    
    
}

KPChorusFlangerAudioProcessor::~KPChorusFlangerAudioProcessor()
{
    if (mCircularBufferLeft !=nullptr){
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    if (mCircularBufferRight !=nullptr){
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const juce::String KPChorusFlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KPChorusFlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KPChorusFlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KPChorusFlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KPChorusFlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KPChorusFlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KPChorusFlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KPChorusFlangerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KPChorusFlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void KPChorusFlangerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KPChorusFlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    //mDelayTimeInSamples = sampleRate * *mTimeParameter;
    //mTimeSmoothed = 1;
    
    /** initialize our data for the current sample rate, and reset things such as phase and writeheads */
    
    /** initialize the phase */
    mLFOPhase = 0;
    
    /** calcualte the circular buffer length */
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;

    /** initialize the left buffer */
    if (mCircularBufferLeft != nullptr ) {
       delete [] mCircularBufferLeft;
       mCircularBufferLeft = nullptr;
    }
    if (mCircularBufferLeft ==nullptr){
        mCircularBufferLeft = new float [(int)mCircularBufferLength];
    }
    /** clear any junk data in new buffer */
    juce::zeromem(mCircularBufferLeft,mCircularBufferLength * sizeof(float));
    
    
    /** initialize the right buffer */
    if (mCircularBufferRight != nullptr ) {
       delete [] mCircularBufferRight;
       mCircularBufferRight = nullptr;
    }
    if (mCircularBufferRight ==nullptr){
        mCircularBufferRight = new float [(int)mCircularBufferLength];
    }
    /** clear any junk data in new buffer */
    juce::zeromem(mCircularBufferRight,mCircularBufferLength * sizeof(float));

    /** initialize the write head */
    mCircularBufferWriteHead = 0;
    
    

    
}

void KPChorusFlangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//#ifndef JucePlugin_PreferredChannelConfigurations
bool KPChorusFlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    //#if JucePlugin_IsMidiEffect
    //  juce::ignoreUnused (layouts);
    //  return true;
    //#else
      // This is the place where you check if the layout is supported.
      // In this template code we only support mono or stereo.
      // Some plugin hosts, such as certain GarageBand versions, will only
      // load plugins that support stereo bus layouts.
      /*if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
       && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
          return false;
      */
      if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo() &&
          layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()) {
          return true;
      } else {
          return false;
      }
      
      // This checks if the input layout matches the output layout
     /*#if ! JucePlugin_IsSynth
      if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
          return false;
     //#endif

      return true;
    //#endif */
  }
  //#endif

void KPChorusFlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    /** obtain the left and right audio data pointer */
    float* leftchannel = buffer.getWritePointer(0);
    float* rightchannel = buffer.getWritePointer(1);
    
    
    /** iterate through all the samples in the buffer */
    for (int i =0; i < buffer.getNumSamples(); i++){
        
        /** write into Circular buffer */
        mCircularBufferLeft[mCircularBufferWriteHead] = leftchannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightchannel[i] + mFeedbackRight;
        
        
        
        /** generate left channel LFO output */
        float lfoOutLeft = sin(2*M_PI * mLFOPhase);
        
        /** calculate the right channel LFO phase */
        
        mPhaseOffsetSmoothed = mPhaseOffsetSmoothed - 0.0001 * (mPhaseOffsetSmoothed - *mPhaseOffsetParameter);
        float lfoPhaseRight = mLFOPhase + mPhaseOffsetSmoothed;
        if (lfoPhaseRight >1){
            lfoPhaseRight -= 1;
        }
        /** generate right channel LFO output */
        float lfoOutRight = sin(2*M_PI * lfoPhaseRight);
        
        /** moving LFO phase forward */
        mRateSmoothed = mRateSmoothed - 0.0001 * (mRateSmoothed - *mRateParameter);
        mLFOPhase += mRateSmoothed / getSampleRate();
        if (mLFOPhase >1){
            mLFOPhase -= 1;
        }
        
        /** calculate the LFO depth */
        mDepthSmoothed = mDepthSmoothed - 0.0001 * (mDepthSmoothed - *mDepthParameter);
        lfoOutLeft *= mDepthSmoothed;
        lfoOutRight *= mDepthSmoothed;
        float lfoOutMappedLeft = 0;
        float lfoOutMappedRight = 0;
        
        /** map our LFO output to our desired delay times */
        
        
        // **Chorus Effect **//
        if (mTypeParameter == 0){
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.005f, 0.03f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);
        }
        
        // **Flanger Effect **//
        else{
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.005f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
        }
        
        /** calculate the delay lengths in samples */
        float delayTimeInSampleLeft = getSampleRate() * lfoOutMappedLeft;
        float delayTimeInSampleRight = getSampleRate() * lfoOutMappedRight;
        
        


        
        //mTimeSmoothed no use now, as the LFO is smoothed waveform
        //mTimeSmoothed = mTimeSmoothed - 0.0001 * (mTimeSmoothed - lfoOutMapped);
        
        /** calculate the left read head position */
        float delayReadHeadLeft = mCircularBufferWriteHead - delayTimeInSampleLeft;
        if (delayReadHeadLeft < 0){
            delayReadHeadLeft += mCircularBufferLength;
        }
        
        /** calculate the right read head position */
        float delayReadHeadRight = mCircularBufferWriteHead - delayTimeInSampleRight;
        if (delayReadHeadRight < 0){
            delayReadHeadRight += mCircularBufferLength;
        }
        
        
        /** calculate linear interpolation points for left channel */
        int readHeadLeft_x = (int)delayReadHeadLeft;
        int readHeadLeft_x1 = readHeadLeft_x + 1;
        float readHeadFloatLeft = delayReadHeadLeft - readHeadLeft_x;
        
        if (readHeadLeft_x1 >= mCircularBufferLength){
            readHeadLeft_x1 -= mCircularBufferLength;
        }
        
        
        /** calculate linear interpolation points for right channel */
        int readHeadRight_x = (int)delayReadHeadRight;
        int readHeadRight_x1 = readHeadRight_x + 1;
        float readHeadFloatRight = delayReadHeadRight - readHeadRight_x;
        
        if (readHeadRight_x1 >= mCircularBufferLength){
            readHeadRight_x1 -= mCircularBufferLength;
        }
        
        /** generate left and right output samples*/
        float delay_sample_left = KPChorusFlangerAudioProcessor::lin_interp(mCircularBufferLeft[readHeadLeft_x], mCircularBufferLeft[readHeadLeft_x1], readHeadFloatLeft);
        float delay_sample_right = KPChorusFlangerAudioProcessor::lin_interp(mCircularBufferRight[readHeadRight_x], mCircularBufferRight[readHeadRight_x1], readHeadFloatRight);
        
        /** write into feedback buffer*/
        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;
        
    
        
        mCircularBufferWriteHead++;

        if (mCircularBufferWriteHead >= mCircularBufferLength){
            mCircularBufferWriteHead = 0;
        }
        
        float dryAmount =1 - *mDryWetParameter;
        float wetAmount = *mDryWetParameter;
        
        buffer.setSample(0, i, buffer.getSample(0, i) * dryAmount + delay_sample_left * wetAmount);
        buffer.setSample(1, i, buffer.getSample(1, i) * dryAmount + delay_sample_right * wetAmount);
    }

    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    //}
}

//==============================================================================
bool KPChorusFlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KPChorusFlangerAudioProcessor::createEditor()
{
    return new KPChorusFlangerAudioProcessorEditor (*this);
}

//==============================================================================
void KPChorusFlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("FlangerChorus"));
    
    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Depth", *mDepthParameter);
    xml->setAttribute("Rate", *mRateParameter);
    xml->setAttribute("Phaseoffset", *mPhaseOffsetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("Type", *mTypeParameter);
    
    copyXmlToBinary(*xml, destData);
    
}

void KPChorusFlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName("FlangerChorus")){
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mDepthParameter = xml->getDoubleAttribute("Depth");
        *mRateParameter = xml->getDoubleAttribute("Rate");
        *mPhaseOffsetParameter = xml->getDoubleAttribute("Phaseoffset");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");
        *mTypeParameter = xml->getIntAttribute("Type");
        
    }
    
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KPChorusFlangerAudioProcessor();
}


float KPChorusFlangerAudioProcessor::lin_interp(float inSampleX, float inSampleY, float inFloatPhase)
{
    return (1-inFloatPhase) * inSampleX + inFloatPhase * inSampleY;
}
