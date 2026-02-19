/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <mutex>

namespace {
    std::once_flag gStkInitOnce;
}

//==============================================================================
LFOTremoloStarterv7AudioProcessor::LFOTremoloStarterv7AudioProcessor()
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
    //chorus params

    juce::NormalisableRange<float> rateRange (0.01f, 10.0f); //min, max in hz
    rateRange.interval = 0.0f; 

    rateRange.skew = 0.6f;   // >1 more low range control, <1 more high end

    addParameter (LFORateUParam = new juce::AudioParameterFloat("lforate", "LFO rate", rateRange, 1.0f));
    
    addParameter (LFODepthUParam = new juce::AudioParameterFloat ("lfodepth", // parameterID
                                                               "LFO Depth", // parameter name
                                                               juce::NormalisableRange<float> (0.0f, 100.0f), //parameter range
                                                               50.0f)); // default value
    
    addParameter (dryWetParam = new juce::AudioParameterFloat ("dryWet", "Dry/Wet", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    
    

    //voice params                                                           
    
    addParameter (bassVoiceGainParam    = new juce::AudioParameterFloat("bassVoiceGain",    "Bass Gain",    juce::NormalisableRange<float>(-60.0f, 6.0f, 0.01f), 0.0f)); //voice gain scaled in dB, -60(later treated as -inf) to +6 range (later soft clipped JIC)
    
    addParameter (bassVoicePanParam = new juce::AudioParameterFloat ("bassVoicePan", "Bass Pan", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f)); // 0 = L, 1 = R, 0.5 = c
    
    addParameter (tenorVoiceGainParam   = new juce::AudioParameterFloat("tenorVoiceGain",   "Tenor Gain",   juce::NormalisableRange<float>(-60.0f, 6.0f, 0.01f), 0.0f));
    
    addParameter (tenorVoicePanParam = new juce::AudioParameterFloat("tenorVoicePan", "Tenor Pan", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    addParameter (altoVoiceGainParam    = new juce::AudioParameterFloat("altoVoiceGain",    "Alto Gain",    juce::NormalisableRange<float>(-60.0f, 6.0f, 0.01f), 0.0f));
    
    addParameter (altoVoicePanParam = new juce::AudioParameterFloat("altoVoicePan", "Alto Pan", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    addParameter (sopranoVoiceGainParam = new juce::AudioParameterFloat("sopranoVoiceGain", "Soprano Gain", juce::NormalisableRange<float>(-60.0f, 6.0f, 0.01f), 0.0f));
    
    addParameter (sopranoVoicePanParam = new juce::AudioParameterFloat("sopranoVoicePan", "Soprano Pan", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    //Pitch shift params
    addParameter(bassPitchSemitones = new juce::AudioParameterFloat("bassPitch", "Bass Pitch", -12.0f, 12.0f, -12.0f));   // min, max, default

    addParameter(tenorPitchSemitones = new juce::AudioParameterFloat("tenorPitch", "Tenor Pitch", -12.0f, 12.0f, -7.0f));

    addParameter(altoPitchSemitones = new juce::AudioParameterFloat("altoPitch", "Alto Pitch", -12.0f, 12.0f, 5.0f));

    addParameter(sopranoPitchSemitones = new juce::AudioParameterFloat("sopranoPitch", "Soprano Pitch", -12.0f, 12.0f, 12.0f));
    
    //toggles
    addParameter(bassPitchEnabledParam = new juce::AudioParameterBool("bassPitchEnabled", "Bass Pitch Shift", false));
    addParameter(tenorPitchEnabledParam = new juce::AudioParameterBool("tenorPitchEnabled", "Tenor Pitch Shift", false));
    addParameter(altoPitchEnabledParam = new juce::AudioParameterBool("altoPitchEnabled", "Alto Pitch Shift", false));
    addParameter(sopranoPitchEnabledParam = new juce::AudioParameterBool("sopranoPitchEnabled", "Soprano Pitch Shift", false));
    
    addParameter(bassMuteParam    = new juce::AudioParameterBool("bassMute",    "Bass Mute",    false));
    addParameter(tenorMuteParam   = new juce::AudioParameterBool("tenorMute",   "Tenor Mute",   false));
    addParameter(altoMuteParam    = new juce::AudioParameterBool("altoMute",    "Alto Mute",    false));
    addParameter(sopranoMuteParam = new juce::AudioParameterBool("sopranoMute", "Soprano Mute", false));

}

LFOTremoloStarterv7AudioProcessor::~LFOTremoloStarterv7AudioProcessor()
{
}

//==============================================================================
const juce::String LFOTremoloStarterv7AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LFOTremoloStarterv7AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LFOTremoloStarterv7AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LFOTremoloStarterv7AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LFOTremoloStarterv7AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LFOTremoloStarterv7AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LFOTremoloStarterv7AudioProcessor::getCurrentProgram()
{
    return 0;
}

void LFOTremoloStarterv7AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String LFOTremoloStarterv7AudioProcessor::getProgramName (int index)
{
    return {};
}

void LFOTremoloStarterv7AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//Set sample rate globally across STK and instances (heap corruption under multiple DAW instances fix)
static std::once_flag stkInitFlag;
//==============================================================================
void LFOTremoloStarterv7AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //fs = sampleRate;
    
    //stk sample rate declare

    stk::Stk::setSampleRate(sampleRate);
    stk::Stk::showWarnings(false);
    
    //set up LFO for each chorus voice
    const float baseDelayMs = 20.0f; // median desired delay length in ms, modify this to adjust chorus effect amount (formerly 15.0)
    const stk::StkFloat baseDelaySamples =
        (stk::StkFloat)((baseDelayMs / 1000.0f) * sampleRate);
        
    //Each voice is given an individual LFO (controlled by master rate/depth params)
    bassChorusVoice    = stk::Chorus(baseDelaySamples * 0.90f); //varies each chorus voices' rate slightly, should mitigate comb filtering, either way creates uniqueness between voices
    tenorChorusVoice   = stk::Chorus(baseDelaySamples * 1.00f);
    altoChorusVoice    = stk::Chorus(baseDelaySamples * 1.10f);
    sopranoChorusVoice = stk::Chorus(baseDelaySamples * 1.20f);
    
    //chorus delay buffer
    bassChorusVoice.clear();
    tenorChorusVoice.clear();
    altoChorusVoice.clear();
    sopranoChorusVoice.clear();

    bassPitchShifter.clear();
    tenorPitchShifter.clear();
    altoPitchShifter.clear();
    sopranoPitchShifter.clear();
    
    //Pitch Shift
    bassPitchShifter.setEffectMix(1.0f); //1=100% wet (toggle pitch on/off)
    
    tenorPitchShifter.setEffectMix(1.0f);
    
    altoPitchShifter.setEffectMix(1.0f);
    
    sopranoPitchShifter.setEffectMix(1.0f);
    
    //chorus smooth
    depthSmoothed.reset(sampleRate, 0.01); // 10 ms 
    rateSmoothed.reset(sampleRate, 0.01); // 10 ms
    
    calcAlgorithmParams();

}

void LFOTremoloStarterv7AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    bassPitchShifter.clear();
    tenorPitchShifter.clear();
    altoPitchShifter.clear();
    sopranoPitchShifter.clear();


    bassChorusVoice.clear();
    tenorChorusVoice.clear();
    altoChorusVoice.clear();
    sopranoChorusVoice.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LFOTremoloStarterv7AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//This function calculates algorithm parameter values from user parameter values.
void LFOTremoloStarterv7AudioProcessor::calcAlgorithmParams()
{   
    //chorus param targeting smooth values
    rateSmoothed.setTargetValue(*LFORateUParam);
    depthSmoothed.setTargetValue(*LFODepthUParam / 100.0f);
    
    float rateHz  = rateSmoothed.getNextValue();
    float depth01 = depthSmoothed.getNextValue();

    //Param setup
    bassChorusVoice.setModFrequency(rateHz);
    bassChorusVoice.setModDepth(depth01);

    tenorChorusVoice.setModFrequency(rateHz);
    tenorChorusVoice.setModDepth(depth01);

    altoChorusVoice.setModFrequency(rateHz);
    altoChorusVoice.setModDepth(depth01);

    sopranoChorusVoice.setModFrequency(rateHz);
    sopranoChorusVoice.setModDepth(depth01);
    
    //two variables due to smoothing
    currentDepth01 = depth01;
}

void LFOTremoloStarterv7AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    const int numSamps = buffer.getNumSamples();
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamps);
    
    //stereo/mono input (check if more than one, only use left channel if mono)  
    const float* inL = buffer.getReadPointer(0);
    const float* inR = (totalNumInputChannels > 1) ? buffer.getReadPointer(1) : nullptr;

    float* outL = buffer.getWritePointer(0);
    float* outR = (totalNumOutputChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    // Before we process a buffer, update the algorithm params!
    calcAlgorithmParams();
    
    auto dbToGain = [](float db)
    {
        return juce::Decibels::decibelsToGain(db, -60.0f); //auto converts (Linear = 10 ^ (gaindB/20)), sets -60dB as floor
    };
    

    const float wet = juce::jlimit<float>(0.0f, 1.0f, *dryWetParam);
    const float dry = 1.0f - wet;
    
    //pan (should automatically decrease gain to about 70% when in center)
    float bassPanL = std::cos(juce::MathConstants<float>::halfPi * (*bassVoicePanParam));
    float bassPanR = std::sin(juce::MathConstants<float>::halfPi * (*bassVoicePanParam));
    
    float tenorPanL = std::cos(juce::MathConstants<float>::halfPi * (*tenorVoicePanParam));
    float tenorPanR = std::sin(juce::MathConstants<float>::halfPi * (*tenorVoicePanParam));
    
    float altoPanL = std::cos(juce::MathConstants<float>::halfPi * (*altoVoicePanParam));
    float altoPanR = std::sin(juce::MathConstants<float>::halfPi * (*altoVoicePanParam));
    
    float sopranoPanL = std::cos(juce::MathConstants<float>::halfPi * (*sopranoVoicePanParam));
    float sopranoPanR = std::sin(juce::MathConstants<float>::halfPi * (*sopranoVoicePanParam));

    const float bassMute    = (bassMuteParam    && bassMuteParam->get())    ? 0.0f : 1.0f; //added as atvsp pointers for automation
    const float tenorMute   = (tenorMuteParam   && tenorMuteParam->get())   ? 0.0f : 1.0f;
    const float altoMute    = (altoMuteParam    && altoMuteParam->get())    ? 0.0f : 1.0f;
    const float sopranoMute = (sopranoMuteParam && sopranoMuteParam->get()) ? 0.0f : 1.0f;
    
    //Pitch shift: convert semitone into 0-1 float that STK can read
    auto semiToRatio = [](float semi)
    {
        return std::pow(2.0f, semi / 12.0f);
    };
    //calculate pitch based on paramter value
    const float bassRatio    = semiToRatio(*bassPitchSemitones);
    const float tenorRatio   = semiToRatio(*tenorPitchSemitones);
    const float altoRatio    = semiToRatio(*altoPitchSemitones);
    const float sopranoRatio = semiToRatio(*sopranoPitchSemitones);

    bassPitchShifter.setShift(bassRatio);
    tenorPitchShifter.setShift(tenorRatio);
    altoPitchShifter.setShift(altoRatio);
    sopranoPitchShifter.setShift(sopranoRatio);
    
    /*
    depth stereo image: depth at 0 = mono, depth at 100 = full width - independant from the voice pan controls, only affects the stereo width created by the desynchronized LFO in L/R channels
    function takes the L/R signal of the voice, plus the calculated monoAmt based on depth value, i.e. collapseStereoToMono(bassL, bassR, monoAmt), outputs updated L/R
    */
    auto collapseStereoToMono = [](float& L, float& R, float monoAmt)
    {
        const float M = 0.5f * (L + R);
        L = L + monoAmt * (M - L);
        R = R + monoAmt * (M - R);
    };
    const float monoAmt = 1.0f - juce::jlimit(0.0f, 1.0f, currentDepth01); //calculates stereo spread equivalent to depth param once per block, depth high = monoAmt low, depth low = monoAmt high
    depthSmoothed.setTargetValue(*LFODepthUParam / 100.0f);
    
    const bool bassPitchOn =
        (bassPitchEnabledParam != nullptr) && bassPitchEnabledParam->get();
    const bool tenorPitchOn =
        (tenorPitchEnabledParam != nullptr) && tenorPitchEnabledParam->get();
    const bool altoPitchOn =
        (altoPitchEnabledParam != nullptr) && altoPitchEnabledParam->get();
    const bool sopranoPitchOn =
        (sopranoPitchEnabledParam != nullptr) && sopranoPitchEnabledParam->get();
    /*
    Signal Flow: Checks input channels, sums wet signal to mono, calculate stereo spread tied to depth value, process each voice and apply pitch shifting if toggled,
    calculate L/R output for each voice, sum each voice to combined wet L/R, finally sum
    */
    for (int samp = 0; samp < numSamps; ++samp)
    {
        //stereo dry signal
        const float dryL = inL[samp];
        const float dryR = inR ? inR[samp] : dryL;
        
        //mono sum wet signal
        const float wetIn = inR ? 0.5f * (dryL + dryR) : dryL; //check for right channel, if present multiple L/R together and cut the gain in half
        
        //depth stereo (loaded per sample to smooth)
        const float depth01 = depthSmoothed.getNextValue();
        const float monoAmt = 1.0f - depth01;

        // === BASS VOICE ===
        float bassProcessed = bassPitchOn ? bassPitchShifter.tick(wetIn) : wetIn; //check for pitch shift, if yes tick, otherwise just pass wet
        bassChorusVoice.tick(bassProcessed);    
        float bassL = bassChorusVoice.lastOut(0);//channel 0 = L
        float bassR = bassChorusVoice.lastOut(1);//channel 1 = R
        
        collapseStereoToMono(bassL, bassR, monoAmt);//for depth/stereo spread mapping, perform collapse function before voice pan being calculated into output, per voice as each is unique LFO
        
        const float bassGainLin    = dbToGain(*bassVoiceGainParam); //convert gain parameter dB to linear
        
        float bassOutL = bassL * bassGainLin * bassPanL * bassMute; //final L/R sum for this voice, with gain converted back to linear
        float bassOutR = bassR * bassGainLin * bassPanR * bassMute;

        // === TENOR VOICE ===
        float tenorProcessed = tenorPitchOn ? tenorPitchShifter.tick(wetIn) : wetIn;
        tenorChorusVoice.tick(tenorProcessed);
        float tenorL = tenorChorusVoice.lastOut(0);
        float tenorR = tenorChorusVoice.lastOut(1);
        
        collapseStereoToMono(tenorL, tenorR, monoAmt);
        
        const float tenorGainLin   = dbToGain(*tenorVoiceGainParam);
        
        float tenorOutL = tenorL * tenorGainLin * tenorPanL * tenorMute;
        float tenorOutR = tenorR * tenorGainLin * tenorPanR * tenorMute;

        
        // === ALTO VOICE ===
        float altoProcessed = altoPitchOn ? altoPitchShifter.tick(wetIn) : wetIn;
        altoChorusVoice.tick(altoProcessed);    
        float altoL = altoChorusVoice.lastOut(0);
        float altoR = altoChorusVoice.lastOut(1);
        
        collapseStereoToMono(altoL, altoR, monoAmt);
        
        const float altoGainLin    = dbToGain(*altoVoiceGainParam);
        
        float altoOutL = altoL * altoGainLin * altoPanL * altoMute;
        float altoOutR = altoR * altoGainLin * altoPanR * altoMute;
        
        // === SOPRANO VOICE ===
        float sopranoProcessed = sopranoPitchOn ? sopranoPitchShifter.tick(wetIn) : wetIn;
        sopranoChorusVoice.tick(sopranoProcessed);
        float sopranoL = sopranoChorusVoice.lastOut(0);
        float sopranoR = sopranoChorusVoice.lastOut(1);
        
        collapseStereoToMono(sopranoL, sopranoR, monoAmt);
        
        const float sopranoGainLin = dbToGain(*sopranoVoiceGainParam);
        
        float sopranoOutL = sopranoL * sopranoGainLin * sopranoPanL * sopranoMute;
        float sopranoOutR = sopranoR * sopranoGainLin * sopranoPanR * sopranoMute;
        
        //wet sum
        const float wetL = bassOutL + tenorOutL + altoOutL + sopranoOutL;
        const float wetR = bassOutR + tenorOutR + altoOutR + sopranoOutR;


        // === FINAL STEREO SUM ===
        float sumL = (dryL * dry) + (wetL * wet);//"dry" and "wet" variables measure amount based on dry/wet param, multiply by fully wet/dry L/R signal to get desired balance
        float sumR = (dryR * dry) + (wetR * wet);

        // soft clip: tanh is a symmetrical clipper (linear near 0, compresses large amplitudes), also introduces saturation
        constexpr float drive = 0.9f; // This is fixed, just in here so I can tweak the saturation coloration in testing
        outL[samp] = std::tanh(sumL* drive);
        outR[samp] = std::tanh(sumR* drive);

    }   
}



//==============================================================================
bool LFOTremoloStarterv7AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LFOTremoloStarterv7AudioProcessor::createEditor()
{
    return new LFOTremoloStarterv7AudioProcessorEditor (*this);
}

//==============================================================================
void LFOTremoloStarterv7AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //for multiple vars.
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("LFOTremoloParams"));
        xml->setAttribute ("lforate", (double) *LFORateUParam);
        xml->setAttribute ("lfodepth", (double) *LFODepthUParam);
        xml->setAttribute ("dryWet", (double) *dryWetParam);
        // Bass
        xml->setAttribute("bassVoiceGain", (double) *bassVoiceGainParam);
        xml->setAttribute("bassVoicePan", (double) *bassVoicePanParam);
        // Tenor
        xml->setAttribute("tenorVoiceGain", (double) *tenorVoiceGainParam);
        xml->setAttribute("tenorVoicePan", (double) *tenorVoicePanParam);

        // Alto
        xml->setAttribute("altoVoiceGain", (double) *altoVoiceGainParam);
        xml->setAttribute("altoVoicePan", (double) *altoVoicePanParam);

        // Soprano
        xml->setAttribute("sopranoVoiceGain", (double) *sopranoVoiceGainParam);
        xml->setAttribute("sopranoVoicePan", (double) *sopranoVoicePanParam);
        // Pitch toggle states
        xml->setAttribute("bassPitchEnabled",    bassPitchEnabledParam    ? bassPitchEnabledParam->get()    : false);
        xml->setAttribute("tenorPitchEnabled",   tenorPitchEnabledParam   ? tenorPitchEnabledParam->get()   : false);
        xml->setAttribute("altoPitchEnabled",    altoPitchEnabledParam    ? altoPitchEnabledParam->get()    : false);
        xml->setAttribute("sopranoPitchEnabled", sopranoPitchEnabledParam ? sopranoPitchEnabledParam->get() : false);
        //pitch shift
        xml->setAttribute("bassPitch", (double) *bassPitchSemitones);
        xml->setAttribute("tenorPitch", (double) *tenorPitchSemitones);
        xml->setAttribute("altoPitch", (double) *altoPitchSemitones);
        xml->setAttribute("sopranoPitch", (double) *sopranoPitchSemitones);
        
        //voice mute
        xml->setAttribute("bassMute",    bassMuteParam->get());
        xml->setAttribute("tenorMute",   tenorMuteParam->get());
        xml->setAttribute("altoMute",    altoMuteParam->get());
        xml->setAttribute("sopranoMute", sopranoMuteParam->get());
        
    copyXmlToBinary (*xml, destData);
}

void LFOTremoloStarterv7AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    //for multiple vars
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName ("LFOTremoloParams"))
        {
            *LFORateUParam = xmlState->getDoubleAttribute ("lforate", 1.0);
            *LFODepthUParam = xmlState->getDoubleAttribute ("lfodepth", 50.0);
            *dryWetParam = xmlState->getDoubleAttribute ("dryWet", 0.5);
            // Bass
            *bassVoiceGainParam   = xmlState->getDoubleAttribute("bassVoiceGain", 0.0);
            *bassVoicePanParam    = xmlState->getDoubleAttribute("bassVoicePan", 0.5);

            // Tenor
            *tenorVoiceGainParam  = xmlState->getDoubleAttribute("tenorVoiceGain", 0.0);
            *tenorVoicePanParam   = xmlState->getDoubleAttribute("tenorVoicePan", 0.5);

            // Alto
            *altoVoiceGainParam   = xmlState->getDoubleAttribute("altoVoiceGain", 0.0);
            *altoVoicePanParam    = xmlState->getDoubleAttribute("altoVoicePan", 0.5);

            // Soprano
            *sopranoVoiceGainParam = xmlState->getDoubleAttribute("sopranoVoiceGain", 0.0);
            *sopranoVoicePanParam  = xmlState->getDoubleAttribute("sopranoVoicePan", 0.5);
            
            //pitch shift
            
            if (bassPitchEnabledParam)
                bassPitchEnabledParam->setValueNotifyingHost(xmlState->getBoolAttribute("bassPitchEnabled", false) ? 1.0f : 0.0f);
            if (tenorPitchEnabledParam)
                tenorPitchEnabledParam->setValueNotifyingHost(xmlState->getBoolAttribute("tenorPitchEnabled", false) ? 1.0f : 0.0f);
            if (altoPitchEnabledParam)
                altoPitchEnabledParam->setValueNotifyingHost(xmlState->getBoolAttribute("altoPitchEnabled", false) ? 1.0f : 0.0f);
            if (sopranoPitchEnabledParam)
                sopranoPitchEnabledParam->setValueNotifyingHost(xmlState->getBoolAttribute("sopranoPitchEnabled", false) ? 1.0f : 0.0f);
            
            *bassPitchSemitones    = (float) xmlState->getDoubleAttribute("bassPitch", -12.0);
            *tenorPitchSemitones   = (float) xmlState->getDoubleAttribute("tenorPitch", -7.0);
            *altoPitchSemitones    = (float) xmlState->getDoubleAttribute("altoPitch", 5.0);
            *sopranoPitchSemitones = (float) xmlState->getDoubleAttribute("sopranoPitch", 12.0);
            
            //voice mute
            if (bassMuteParam)
                bassMuteParam->setValueNotifyingHost(xmlState->getBoolAttribute("bassMute", false) ? 1.0f : 0.0f);
            if (tenorMuteParam)
                tenorMuteParam->setValueNotifyingHost(xmlState->getBoolAttribute("tenorMute", false) ? 1.0f : 0.0f);    
            if (altoMuteParam)
                altoMuteParam->setValueNotifyingHost(xmlState->getBoolAttribute("altoMute", false) ? 1.0f : 0.0f);
            if (sopranoMuteParam)
                sopranoMuteParam->setValueNotifyingHost(xmlState->getBoolAttribute("sopranoMute", false) ? 1.0f : 0.0f);

        }
    }
}
//==============================================================================
//pitch shift toggles
void LFOTremoloStarterv7AudioProcessor::setBassPitchShiftEnabled(bool shouldEnable)
{
    bassPitchShiftEnabled = shouldEnable;
}
void LFOTremoloStarterv7AudioProcessor::setTenorPitchShiftEnabled(bool shouldEnable)
{
    tenorPitchShiftEnabled = shouldEnable;
}
void LFOTremoloStarterv7AudioProcessor::setAltoPitchShiftEnabled(bool shouldEnable)
{
    altoPitchShiftEnabled = shouldEnable;
}

void LFOTremoloStarterv7AudioProcessor::setSopranoPitchShiftEnabled(bool shouldEnable)
{
    sopranoPitchShiftEnabled = shouldEnable;
}

bool LFOTremoloStarterv7AudioProcessor::getBassPitchShiftEnabled() const
{
    return bassPitchShiftEnabled;
}

bool LFOTremoloStarterv7AudioProcessor::getTenorPitchShiftEnabled() const
{
    return tenorPitchShiftEnabled;
}

bool LFOTremoloStarterv7AudioProcessor::getAltoPitchShiftEnabled() const
{
    return altoPitchShiftEnabled;
}

bool LFOTremoloStarterv7AudioProcessor::getSopranoPitchShiftEnabled() const
{
    return sopranoPitchShiftEnabled;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LFOTremoloStarterv7AudioProcessor();
}