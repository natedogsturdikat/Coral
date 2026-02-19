/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/**
*/
class LFOTremoloStarterv7AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                public juce::Slider::Listener,
                                                public juce::Timer,
                                                public juce::Button::Listener
{
public:
    LFOTremoloStarterv7AudioProcessorEditor (LFOTremoloStarterv7AudioProcessor&);
    ~LFOTremoloStarterv7AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider*) override;
    void buttonClicked(juce::Button* button) override;
    void updatePanPitchView();  
    void timerCallback() override;

private:
    //chorus section sliders:
    juce::Slider LFOrate;    
    juce::Slider LFOdepth;
    juce::Label LFOrateTitle; 
    juce::Label LFOdepthTitle;
    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;
    
    // GUI Labels
    juce::Label chorusSection;
    
    //individual voice sections:
    juce::Label sopranoSection;
    juce::Label altoSection;
    juce::Label tenorSection;
    juce::Label bassSection;
    
    // rate slider labels
    juce::Label rateTextL;
    juce::Label rateTextR;
    
    // The 'area' variable used in resized()
    juce::Rectangle<int> area;
    
    //voice section sliders:
    
    //bass
    juce::Slider bassGainSlider;
    juce::Slider bassPanSlider;
    juce::Label bassGainLabel;
    juce::Label bassPanLabel;
    juce::ToggleButton bassPitchToggle;
    juce::TextButton bassSectionLabel;
    
    //tenor
    juce::Slider tenorGainSlider;
    juce::Slider tenorPanSlider;
    juce::Label tenorGainLabel;
    juce::Label tenorPanLabel;
    juce::ToggleButton tenorPitchToggle;
    juce::TextButton tenorSectionLabel;
    
    // Alto controls
    juce::Slider altoGainSlider;
    juce::Slider altoPanSlider;
    juce::ToggleButton altoPitchToggle;
    juce::Label altoGainLabel;
    juce::Label altoPanLabel;
    juce::TextButton altoSectionLabel;
    
    // Soprano controls 
    juce::Slider sopranoGainSlider;
    juce::Slider sopranoPanSlider;
    juce::ToggleButton sopranoPitchToggle;
    juce::Label sopranoGainLabel;
    juce::Label sopranoPanLabel;
    juce::TextButton sopranoSectionLabel;
    
    //custom look and feel
    CustomLookAndFeel customLookAndFeel;
    
    //pan/pitch toggle
    juce::Slider bassPitchSlider;

    juce::Slider tenorPitchSlider;

    juce::Slider altoPitchSlider;

    juce::Slider sopranoPitchSlider;

    juce::TextButton panPitchViewButton;
    bool showPitchView = false;
    
    //mute
    void refreshMuteUI();    
    
    //backdrop
    juce::Image reefImage;
    juce::ImageComponent reefComponent;
        
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LFOTremoloStarterv7AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOTremoloStarterv7AudioProcessorEditor)
};