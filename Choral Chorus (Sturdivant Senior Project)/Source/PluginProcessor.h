/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "basicLFO/basicLFO.h"
#include "stk/Chorus.h"
#include "stk/PitShift.h"

//==============================================================================
/**
*/
class LFOTremoloStarterv7AudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    LFOTremoloStarterv7AudioProcessor();
    ~LFOTremoloStarterv7AudioProcessor() override;

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
    
    //Chorus parameter pointers
    juce::AudioParameterFloat* LFORateUParam;
    juce::AudioParameterFloat* LFODepthUParam;
    juce::AudioParameterFloat* dryWetParam = nullptr;
    
    // Individual Voice Parameter pointers
    juce::AudioParameterFloat* bassVoiceGainParam = nullptr;
    juce::AudioParameterFloat* bassVoicePanParam = nullptr;
    void setBassPitchShiftEnabled(bool shouldEnable);

    juce::AudioParameterFloat* tenorVoiceGainParam = nullptr;
    juce::AudioParameterFloat* tenorVoicePanParam = nullptr;
    void setTenorPitchShiftEnabled(bool shouldEnable);
    
    juce::AudioParameterFloat* altoVoiceGainParam = nullptr;
    juce::AudioParameterFloat* altoVoicePanParam = nullptr;
    void setAltoPitchShiftEnabled(bool shouldEnable);
    
    juce::AudioParameterFloat* sopranoVoiceGainParam = nullptr;
    juce::AudioParameterFloat* sopranoVoicePanParam = nullptr;
    void setSopranoPitchShiftEnabled(bool shouldEnable);
    
    bool getBassPitchShiftEnabled() const;
    bool getTenorPitchShiftEnabled() const;
    bool getAltoPitchShiftEnabled() const;
    bool getSopranoPitchShiftEnabled() const;
    
    juce::AudioParameterBool* bassPitchEnabledParam = nullptr;
    juce::AudioParameterBool* tenorPitchEnabledParam = nullptr;
    juce::AudioParameterBool* altoPitchEnabledParam = nullptr;
    juce::AudioParameterBool* sopranoPitchEnabledParam = nullptr;
    
    //voice mutes (Bools for how editor handles the toggle, attached to processor)
    void setBassMuted(bool m)    { bassMuted = m; }
    void setTenorMuted(bool m)   { tenorMuted = m; }
    void setAltoMuted(bool m)    { altoMuted = m; }
    void setSopranoMuted(bool m) { sopranoMuted = m; }

    bool getBassMuted() const    { return bassMuted; }
    bool getTenorMuted() const   { return tenorMuted; }
    bool getAltoMuted() const    { return altoMuted; }
    bool getSopranoMuted() const { return sopranoMuted; }
    
    //audio parameters so Mute toggle can be automated in DAW
    juce::AudioParameterBool* bassMuteParam = nullptr;
    juce::AudioParameterBool* tenorMuteParam = nullptr;
    juce::AudioParameterBool* altoMuteParam = nullptr;
    juce::AudioParameterBool* sopranoMuteParam = nullptr;
    
    juce::AudioParameterFloat* bassPitchSemitones;
    juce::AudioParameterFloat* tenorPitchSemitones;
    juce::AudioParameterFloat* altoPitchSemitones;
    juce::AudioParameterFloat* sopranoPitchSemitones;

private:
    
    //Create any methods you will need
    void calcAlgorithmParams();
    
    // STK Chorus Effect
    std::unique_ptr<stk::Chorus> chorusEffect;
    stk::Chorus bassChorusVoice;
    stk::Chorus tenorChorusVoice;
    stk::Chorus altoChorusVoice;
    stk::Chorus sopranoChorusVoice;
    
    //STK Pitch shift
    stk::PitShift bassPitchShifter;
    stk::PitShift tenorPitchShifter;
    stk::PitShift altoPitchShifter;
    stk::PitShift sopranoPitchShifter;
    
    //LFO variables
    float fs;
    //basicLFO tremLFO;
    float LFODepthAParam;
    float finalGain;
    
    //smoothing for chorus params
    juce::SmoothedValue<float> depthSmoothed;
    juce::SmoothedValue<float> rateSmoothed;
    
    // gain and pan values for the voice
    float bassVoiceGain = 1.0f;
    float bassVoicePan = 0.5f; // 0 = Left, 1 = Right, 0.5 = Center
    
    float tenorVoiceGain = 1.0f;
    float tenorVoicePan = 0.5f; 
    
    float altoVoiceGain = 1.0f;
    float altoVoicePan = 0.5f; 
    
    float sopranoVoiceGain = 1.0f;
    float sopranoVoicePan = 0.5f; 
    
    //pitch toggle buttons
    bool bassPitchShiftEnabled = false;
    bool tenorPitchShiftEnabled = false;
    bool altoPitchShiftEnabled = false;
    bool sopranoPitchShiftEnabled = false;
    
    //voice mute
    bool bassMuted    = false;
    bool tenorMuted   = false;
    bool altoMuted    = false;
    bool sopranoMuted = false;
    
    //pitch shift prep buffer
    int pitchShiftSampleCounter = 0;
    
    //depth stereo image
    float currentDepth01 = 0.0f;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOTremoloStarterv7AudioProcessor)
};
