/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LFOTremoloStarterv7AudioProcessorEditor::LFOTremoloStarterv7AudioProcessorEditor (LFOTremoloStarterv7AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //Areas Visibility
    addAndMakeVisible(chorusSection); //"Areas" for resized later, so components can be centered off of eachother and by section-not visible over png background, for scaling
    addAndMakeVisible(sopranoSection);
    addAndMakeVisible(altoSection);
    addAndMakeVisible(tenorSection);
    addAndMakeVisible(bassSection);
    
    //background png
    reefImage = juce::ImageCache::getFromMemory(BinaryData::reef12_png, BinaryData::reef12_pngSize);
    if (reefImage.isValid())
    {
        reefComponent.setImage(reefImage, juce::RectanglePlacement::stretchToFit);
        addAndMakeVisible(reefComponent); 
        reefComponent.setAlpha(0.95f);//basic brightness/opacity for visbility of sliders/labels
        reefComponent.setInterceptsMouseClicks(false, false);
        
    }
    setLookAndFeel(&customLookAndFeel);

//==============================================================================
//chorus param labels
    //LFO Rate (HZ)
    LFOrate.setRange(0.01, 10.0, 0.0); //Hz (first value formerly 0.1, for small interval steps, set to 0.0 for continous)
    LFOrate.setSliderStyle(juce::Slider::Rotary);
    LFOrate.setSkewFactorFromMidPoint(1.0); 
    LFOrate.setValue(2.0);
    LFOrate.setNumDecimalPlacesToDisplay(2);
    LFOrate.setTextValueSuffix(" hz");
    LFOrate.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    LFOrate.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    LFOrate.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    LFOrate.addListener(this);
    addAndMakeVisible(LFOrate);

    //Rate label
    LFOrateTitle.setText("Rate", juce::dontSendNotification);
    //LFOrateTitle.attachToComponent(&LFOrate, false);
    LFOrateTitle.setColour(juce::Label::textColourId, juce::Colours::white);
    LFOrateTitle.setFont(juce::Font(18.0));
    LFOrateTitle.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(LFOrateTitle);
    
    // "tight" label (left side of rate slider)
    rateTextL.setText("tight", juce::dontSendNotification);
    rateTextL.setColour(juce::Label::textColourId, juce::Colours::white);
    rateTextL.setFont(juce::Font(14.0f));
    rateTextL.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(rateTextL);

    // "messy" label (right side of rate slider)
    rateTextR.setText("messy", juce::dontSendNotification);
    rateTextR.setColour(juce::Label::textColourId, juce::Colours::white);
    rateTextR.setFont(juce::Font(14.0f));
    rateTextR.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rateTextR);

    //LFO Depth (0-100 %)
    LFOdepth.setSliderStyle (juce::Slider::Rotary);
    LFOdepth.setRange(0.0, 100, 0.5); //0-100 is %
    LFOdepth.setValue(50);
    LFOdepth.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    LFOdepth.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    LFOdepth.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    LFOdepth.setPopupDisplayEnabled (true, true, this);
    LFOdepth.setTextValueSuffix(" %");
    LFOdepth.addListener (this);
    addAndMakeVisible(LFOdepth);

    //Depth label
    LFOdepthTitle.setText("Depth", juce::dontSendNotification);
    LFOdepthTitle.setColour(juce::Label::textColourId, juce::Colours::white);
    LFOdepthTitle.setFont(juce::Font(18.0));
    LFOdepthTitle.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(LFOdepthTitle);
    
    // Dry/Wet slider (0 = dry, 100 = wet)
    dryWetSlider.setSliderStyle(juce::Slider::Rotary);
    dryWetSlider.setRange(0.0, 100.0, 1.0);
    dryWetSlider.setValue(100.0);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    dryWetSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    dryWetSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dryWetSlider.setTextValueSuffix(" %");
    dryWetSlider.setPopupDisplayEnabled(true, true, this);
    dryWetSlider.addListener(this);
    addAndMakeVisible(dryWetSlider);

    // Dry/Wet label
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dryWetLabel.setFont(juce::Font(18.0f));
    dryWetLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(dryWetLabel);

//==============================================================================    
    //voice section sliders:
    
    //bass
    // Bass Gain Fader
    bassGainSlider.setSliderStyle(juce::Slider::LinearVertical); //fader style as opposed to rotary pot style
    //bassGainSlider.setRange(0.0, 1.0, 0.01);
    bassGainSlider.setRange(-60.0, 6.0, 0.01); //dB range
    bassGainSlider.setNumDecimalPlacesToDisplay(1);
    bassGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 26); //(originally 60,20)
    bassGainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    bassGainSlider.addListener(this);
    addAndMakeVisible(bassGainSlider);

    bassGainLabel.setText("Gain", juce::dontSendNotification);
    bassGainLabel.attachToComponent(&bassGainSlider, false);
    bassGainLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(bassGainLabel);

    // Bass Pan Rotary
    bassPanSlider.setSliderStyle(juce::Slider::Rotary);
    bassPanSlider.setRange(0.0, 1.0, 0.01);  // 0 = Left, 1 = Right
    bassPanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);//originally 45, 15
    bassPanSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack); //make textbox outline transparent
    bassPanSlider.setTextValueSuffix(""); // optional
    bassPanSlider.addListener(this);
    addAndMakeVisible(bassPanSlider);

    //bassPanLabel.setText("Pan", juce::dontSendNotification);
    bassPanLabel.setText(showPitchView ? "Pitch" : "Pan", juce::dontSendNotification);
    bassPanLabel.attachToComponent(&bassPanSlider, false);
    bassPanLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(bassPanLabel);
    
    // Bass Pitch Toggle Button
    bassPitchToggle.setButtonText("Shift"); //"Pitch Shift" and "transpose" too large for textbox, settled on shift
    bassPitchToggle.setClickingTogglesState(true); // Enable toggle behavior
    bassPitchToggle.addListener(this); 
    addAndMakeVisible(bassPitchToggle);
    
    //Bass Section label
    bassSectionLabel.setButtonText("Bass");
    bassSectionLabel.setClickingTogglesState(true);
    bassSectionLabel.addListener(this);

    // match label
    bassSectionLabel.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    bassSectionLabel.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    bassSectionLabel.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    bassSectionLabel.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(bassSectionLabel);
    
    //Tenor
    // Tenor Gain Fader
    tenorGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    //tenorGainSlider.setRange(0.0, 1.0, 0.01);
    tenorGainSlider.setRange(-60.0, 6.0, 0.01);
    tenorGainSlider.setNumDecimalPlacesToDisplay(1);
    tenorGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 26);
    tenorGainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    tenorGainSlider.addListener(this);
    addAndMakeVisible(tenorGainSlider);
    
    tenorGainLabel.setText("Gain", juce::dontSendNotification);
    tenorGainLabel.attachToComponent(&tenorGainSlider, false);
    tenorGainLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(tenorGainLabel);

    // Tenor Pan Rotary
    tenorPanSlider.setSliderStyle(juce::Slider::Rotary);
    tenorPanSlider.setRange(0.0, 1.0, 0.01);
    tenorPanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    tenorPanSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    tenorPanSlider.addListener(this);
    addAndMakeVisible(tenorPanSlider);

    //tenorPanLabel.setText("Pan", juce::dontSendNotification);
    tenorPanLabel.setText(showPitchView ? "Pitch" : "Pan", juce::dontSendNotification);
    tenorPanLabel.attachToComponent(&tenorPanSlider, false);
    tenorPanLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(tenorPanLabel);

    // Tenor Pitch Toggle Button
    //tenorPitchToggle.setButtonText("v Fifth");
    tenorPitchToggle.setButtonText("Shift");
    tenorPitchToggle.setClickingTogglesState(true);
    tenorPitchToggle.addListener(this);
    addAndMakeVisible(tenorPitchToggle);

    // Tenor Section label
    tenorSectionLabel.setButtonText("Tenor");
    tenorSectionLabel.setClickingTogglesState(true);
    tenorSectionLabel.addListener(this);

    tenorSectionLabel.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    tenorSectionLabel.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    tenorSectionLabel.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    tenorSectionLabel.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(tenorSectionLabel);
    
    //Alto
    // Alto Gain Fader
    altoGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    //altoGainSlider.setRange(0.0, 1.0, 0.01);
    altoGainSlider.setRange(-60.0, 6.0, 0.01);
    altoGainSlider.setNumDecimalPlacesToDisplay(1);
    altoGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 26);
    altoGainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    altoGainSlider.addListener(this);
    addAndMakeVisible(altoGainSlider);

    altoGainLabel.setText("Gain", juce::dontSendNotification);
    altoGainLabel.attachToComponent(&altoGainSlider, false);
    altoGainLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(altoGainLabel);

    // Alto Pan Rotary
    altoPanSlider.setSliderStyle(juce::Slider::Rotary);
    altoPanSlider.setRange(0.0, 1.0, 0.01);
    altoPanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    altoPanSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    altoPanSlider.addListener(this);
    addAndMakeVisible(altoPanSlider);

    altoPanLabel.setText(showPitchView ? "Pitch" : "Pan", juce::dontSendNotification);
    altoPanLabel.attachToComponent(&altoPanSlider, false);
    altoPanLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(altoPanLabel);
    
    // Alto Pitch Toggle Button
    altoPitchToggle.setButtonText("Shift");
    altoPitchToggle.setClickingTogglesState(true);
    altoPitchToggle.addListener(this);
    addAndMakeVisible(altoPitchToggle);

    // Alto Section label
    altoSectionLabel.setButtonText("Alto");
    altoSectionLabel.setClickingTogglesState(true);
    altoSectionLabel.addListener(this);

    altoSectionLabel.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    altoSectionLabel.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    altoSectionLabel.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    altoSectionLabel.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(altoSectionLabel);
    
    //Soprano
    // Soprano Gain Fader
    sopranoGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    //sopranoGainSlider.setRange(0.0, 1.0, 0.01);
    sopranoGainSlider.setRange(-60.0, 6.0, 0.01);
    sopranoGainSlider.setNumDecimalPlacesToDisplay(1);
    sopranoGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 26);
    sopranoGainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    sopranoGainSlider.addListener(this);
    addAndMakeVisible(sopranoGainSlider);

    sopranoGainLabel.setText("Gain", juce::dontSendNotification);
    sopranoGainLabel.attachToComponent(&sopranoGainSlider, false);
    sopranoGainLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(sopranoGainLabel);

    // Soprano Pan Rotary
    sopranoPanSlider.setSliderStyle(juce::Slider::Rotary);
    sopranoPanSlider.setRange(0.0, 1.0, 0.01);
    sopranoPanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    sopranoPanSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    sopranoPanSlider.addListener(this);
    addAndMakeVisible(sopranoPanSlider);

    sopranoPanLabel.setText(showPitchView ? "Pitch" : "Pan", juce::dontSendNotification);
    sopranoPanLabel.attachToComponent(&sopranoPanSlider, false);
    sopranoPanLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(sopranoPanLabel);
    
    // Soprano Pitch Toggle Button
    sopranoPitchToggle.setButtonText("Shift");
    sopranoPitchToggle.setClickingTogglesState(true);
    sopranoPitchToggle.addListener(this);
    addAndMakeVisible(sopranoPitchToggle);

    // Soprano Section label
    sopranoSectionLabel.setButtonText("Soprano");
    sopranoSectionLabel.setClickingTogglesState(true);
    sopranoSectionLabel.addListener(this);

    sopranoSectionLabel.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    sopranoSectionLabel.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    sopranoSectionLabel.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sopranoSectionLabel.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(sopranoSectionLabel);
    
    auto dbText = [](double lvl)
    {
        if (lvl <= -60.0)
            return juce::String("-inf");
        return juce::String(lvl, 1) + " dB";
    };

    bassGainSlider.textFromValueFunction = dbText;
    tenorGainSlider.textFromValueFunction = dbText;
    altoGainSlider.textFromValueFunction = dbText;
    sopranoGainSlider.textFromValueFunction = dbText;

//==============================================================================
    //pan/pitch section
    addAndMakeVisible(panPitchViewButton);
    panPitchViewButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    panPitchViewButton.setColour(juce::TextButton::textColourOnId,  juce::Colours::white);
    panPitchViewButton.setClickingTogglesState(true);
    panPitchViewButton.addListener(this);
    juce::Colour pitchPurple (0xff7fb3ff); //slider thumb color diff from customLookAndFeel to distinguish from pan rotary
    
    bassPitchSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    tenorPitchSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    altoPitchSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    sopranoPitchSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    
    bassPitchSlider.setTextValueSuffix(" st");
    tenorPitchSlider.setTextValueSuffix(" st");
    altoPitchSlider.setTextValueSuffix(" st");
    sopranoPitchSlider.setTextValueSuffix(" st");
    
    addAndMakeVisible(bassPitchSlider);
    addAndMakeVisible(tenorPitchSlider);
    addAndMakeVisible(altoPitchSlider);
    addAndMakeVisible(sopranoPitchSlider);

    // range value
    bassPitchSlider.setRange(-12, 12, 1);
    tenorPitchSlider.setRange(-12, 12, 1);
    altoPitchSlider.setRange(-12, 12, 1);
    sopranoPitchSlider.setRange(-12, 12, 1);

    bassPitchSlider.setSliderStyle(juce::Slider::Rotary);
    bassPitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    bassPitchSlider.setColour(juce::Slider::thumbColourId, pitchPurple);
    
    tenorPitchSlider.setSliderStyle(juce::Slider::Rotary);
    tenorPitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    tenorPitchSlider.setColour(juce::Slider::thumbColourId, pitchPurple);
    
    altoPitchSlider.setSliderStyle(juce::Slider::Rotary);
    altoPitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    altoPitchSlider.setColour(juce::Slider::thumbColourId, pitchPurple);
    
    sopranoPitchSlider.setSliderStyle(juce::Slider::Rotary);
    sopranoPitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 20);
    sopranoPitchSlider.setColour(juce::Slider::thumbColourId, pitchPurple);
    
    //defaults: octave up, fifth up, lower fifth, octave down
    bassPitchSlider.setValue(-12, juce::dontSendNotification);
    tenorPitchSlider.setValue(-5,  juce::dontSendNotification);
    altoPitchSlider.setValue(7,    juce::dontSendNotification);
    sopranoPitchSlider.setValue(12, juce::dontSendNotification);
    
    //sliders
    bassPitchSlider.addListener(this);
    tenorPitchSlider.addListener(this);
    altoPitchSlider.addListener(this);
    sopranoPitchSlider.addListener(this);

    showPitchView = false; //defaults to pan view
    updatePanPitchView();
    

    
//==============================================================================    
    //set GUI to default params
    LFOrate.setValue(*audioProcessor.LFORateUParam, juce::dontSendNotification);
    LFOdepth.setValue(*audioProcessor.LFODepthUParam, juce::dontSendNotification);
    dryWetSlider.setValue(*audioProcessor.dryWetParam * 100.0f, juce::dontSendNotification);
    
    bassGainSlider.setValue(*audioProcessor.bassVoiceGainParam, juce::dontSendNotification);
    bassPanSlider.setValue(*audioProcessor.bassVoicePanParam, juce::dontSendNotification);
    tenorGainSlider.setValue(*audioProcessor.tenorVoiceGainParam, juce::dontSendNotification);
    tenorPanSlider.setValue(*audioProcessor.tenorVoicePanParam, juce::dontSendNotification);
    altoGainSlider.setValue(*audioProcessor.altoVoiceGainParam, juce::dontSendNotification);
    altoPanSlider.setValue(*audioProcessor.altoVoicePanParam, juce::dontSendNotification);
    sopranoGainSlider.setValue(*audioProcessor.sopranoVoiceGainParam, juce::dontSendNotification);
    sopranoPanSlider.setValue(*audioProcessor.sopranoVoicePanParam, juce::dontSendNotification);
    
    bassPitchToggle.setToggleState(audioProcessor.getBassPitchShiftEnabled(), juce::dontSendNotification);
    tenorPitchToggle.setToggleState(audioProcessor.getTenorPitchShiftEnabled(), juce::dontSendNotification);
    altoPitchToggle.setToggleState(audioProcessor.getAltoPitchShiftEnabled(), juce::dontSendNotification);
    sopranoPitchToggle.setToggleState(audioProcessor.getSopranoPitchShiftEnabled(), juce::dontSendNotification);
    
    bassPitchSlider.setValue(*audioProcessor.bassPitchSemitones, juce::dontSendNotification);
    tenorPitchSlider.setValue(*audioProcessor.tenorPitchSemitones, juce::dontSendNotification);
    altoPitchSlider.setValue(*audioProcessor.altoPitchSemitones, juce::dontSendNotification);
    sopranoPitchSlider.setValue(*audioProcessor.sopranoPitchSemitones, juce::dontSendNotification);

    
    //double click reset (quickly set params back to defaults)
    bassPanSlider.setDoubleClickReturnValue(true, 0.5f);
    tenorPanSlider.setDoubleClickReturnValue(true, 0.5f);
    altoPanSlider.setDoubleClickReturnValue(true, 0.5f);
    sopranoPanSlider.setDoubleClickReturnValue(true, 0.5f);
    
    /*
    bassGainSlider.setDoubleClickReturnValue(true, 0.75f);
    tenorGainSlider.setDoubleClickReturnValue(true, 0.75f);
    altoGainSlider.setDoubleClickReturnValue(true, 0.75f);
    sopranoGainSlider.setDoubleClickReturnValue(true, 0.75f);
    */
    bassGainSlider.setDoubleClickReturnValue(true, 0.0f);
    tenorGainSlider.setDoubleClickReturnValue(true, 0.0f);
    altoGainSlider.setDoubleClickReturnValue(true, 0.0f);
    sopranoGainSlider.setDoubleClickReturnValue(true, 0.0f);
    
    LFOdepth.setDoubleClickReturnValue(true, 50);
    LFOrate.setDoubleClickReturnValue(true, 1.0);
    dryWetSlider.setDoubleClickReturnValue(true, 100);
    
    bassPitchSlider.setDoubleClickReturnValue(true, -12);
    tenorPitchSlider.setDoubleClickReturnValue(true, -5);
    altoPitchSlider.setDoubleClickReturnValue(true, 7);
    sopranoPitchSlider.setDoubleClickReturnValue(true, 12);
    
    //custom look declarations
    LFOrate.setLookAndFeel(&customLookAndFeel);
    LFOdepth.setLookAndFeel(&customLookAndFeel);
    dryWetSlider.setLookAndFeel(&customLookAndFeel);

    bassPanSlider.setLookAndFeel(&customLookAndFeel);
    tenorPanSlider.setLookAndFeel(&customLookAndFeel);
    altoPanSlider.setLookAndFeel(&customLookAndFeel);
    sopranoPanSlider.setLookAndFeel(&customLookAndFeel);
    
    bassGainSlider.setLookAndFeel(&customLookAndFeel);
    tenorGainSlider.setLookAndFeel(&customLookAndFeel);
    altoGainSlider.setLookAndFeel(&customLookAndFeel);
    sopranoGainSlider.setLookAndFeel(&customLookAndFeel);
    panPitchViewButton.setLookAndFeel(&customLookAndFeel);
    
    //pan/pitch view
    updatePanPitchView();
    
    //refreshMuteUI
    refreshMuteUI();
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 500);
    //startTimer(100);    // This sets how often (in msec) the timerCallback() gets called.
    startTimerHz(30); //refreshes 30 times per second as opposed to a ms interval, slightly faster but more of just a standard thing for GUIs
}

LFOTremoloStarterv7AudioProcessorEditor::~LFOTremoloStarterv7AudioProcessorEditor()
{
    stopTimer();
    LFOrate.setLookAndFeel(nullptr);
    LFOdepth.setLookAndFeel(nullptr);
    dryWetSlider.setLookAndFeel(nullptr);

    bassPanSlider.setLookAndFeel(nullptr);
    tenorPanSlider.setLookAndFeel(nullptr);
    altoPanSlider.setLookAndFeel(nullptr);
    sopranoPanSlider.setLookAndFeel(nullptr);
    
    bassGainSlider.setLookAndFeel(nullptr);
    tenorGainSlider.setLookAndFeel(nullptr);
    altoGainSlider.setLookAndFeel(nullptr);
    sopranoGainSlider.setLookAndFeel(nullptr);
    
    panPitchViewButton.setLookAndFeel(nullptr);
    
    setLookAndFeel(nullptr);
}

//==============================================================================
void LFOTremoloStarterv7AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Coral", getLocalBounds(), juce::Justification::centredBottom, 1);
    
}

void LFOTremoloStarterv7AudioProcessorEditor::resized()
{
    
    //chorus section slider spacing, reduced all x positions by 20 pixels (formerly 40,160,280)
    int w = 120; int h = 100;
    LFOrate.setBounds(20, 20, w, h);
    LFOdepth.setBounds(140, 20, w, h);
    dryWetSlider.setBounds(260, 20, w, h);//originally 280, 50
    
    LFOrateTitle.setBounds(
        LFOrate.getX(),
        LFOrate.getY() - 16,
        LFOrate.getWidth(),
        20);
    
    LFOdepthTitle.setBounds(
        LFOdepth.getX(),
        LFOdepth.getY() - 16,  
        LFOdepth.getWidth(),
        20);
    dryWetLabel.setBounds(
        dryWetSlider.getX(),
        dryWetSlider.getY() - 16,
        LFOrate.getWidth(),
        20);
        
    rateTextL.setBounds(20, 80, 40, 15);   // (x, y, width, height) (adjusted by -20 x)
    rateTextR.setBounds(105, 80, 40, 15); // Position relative to slider

    auto area = getLocalBounds();
    //larger area sizing
    
    auto chorusArea = area.removeFromTop(getHeight() / 3);
    auto voicesArea = area; //since it's declared after chorusArea, automatically uses remaining space (bottom 2/3)

    //chorus section:
    chorusSection.setBounds(chorusArea);
    chorusSection.setColour(juce::Label::backgroundColourId,
                        customLookAndFeel.getDepthMappedBlue((float)LFOdepth.getValue()));


    //voices sections 
    auto voicesWidth = getWidth() / 4;
    //sop
    sopranoSection.setBounds(voicesArea.removeFromLeft(voicesWidth));
    //alto
    altoSection.setBounds(voicesArea.removeFromLeft(voicesWidth));
    //tenor
    tenorSection.setBounds(voicesArea.removeFromLeft(voicesWidth));
    //bass
    bassSection.setBounds(voicesArea.removeFromLeft(voicesWidth));

    //voice colors
    sopranoSection.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colour(0xffd2b48c));
    altoSection.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colour(0xffd2b48c));
    tenorSection.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colour(0xffd2b48c));
    bassSection.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colour(0xffd2b48c));
    
    //background scaling
    auto reefBounds = getLocalBounds(); // Full area
    reefComponent.setBounds(reefBounds); // Make it fill the entire GUI or part of it
    //voice section sliders
    
    //bass
    int sliderMargin = 10; //simple constant to evenly space sliders, quick adjustments
    int sliderWidth = 60;
    int sliderHeight = 120;
    
    int bassX = bassSection.getX() + sliderMargin;
    int bassY = bassSection.getY() + sliderMargin;

    // Bass Gain
    bassGainSlider.setBounds(
        bassX,
        bassY + 125,
        sliderWidth,
        sliderHeight);
        
    // Bass Pan
    bassPanSlider.setBounds(
        bassX,
        bassY + 25,
        sliderWidth,
        80);
    
    // Bass Pitch Button
    bassPitchToggle.setBounds(
        bassX,
        bassGainSlider.getBottom() + 10,
        sliderWidth,
        25);
    
    // Bass Section Label
    bassSectionLabel.setBounds(
        bassX,
        bassPitchToggle.getBottom() + 5,
        sliderWidth,
        20);
    
    //tenor
    int tenorX = tenorSection.getX() + sliderMargin;
    int tenorY = tenorSection.getY() + sliderMargin;

    // Tenor Gain
    tenorGainSlider.setBounds(
        tenorX,
        tenorY + 125,
        sliderWidth,
        sliderHeight);

    // Tenor Pan
    tenorPanSlider.setBounds(
        tenorX,
        tenorY + 25,
        sliderWidth,
        80);

    // Tenor Pitch Button
    tenorPitchToggle.setBounds(
        tenorX,
        tenorGainSlider.getBottom() + 10,
        sliderWidth,
        25);

    // Tenor Section Label
    tenorSectionLabel.setBounds(
        tenorX,
        tenorPitchToggle.getBottom() + 5,
        sliderWidth,
        20);
    
    // Soprano
    int altoX = altoSection.getX() + sliderMargin;
    int altoY = altoSection.getY() + sliderMargin;

    // Alto Gain
    altoGainSlider.setBounds(
        altoX,
        altoY + 125,
        sliderWidth,
        sliderHeight);

    // Alto Pan
    altoPanSlider.setBounds(
        altoX,
        altoY + 25,
        sliderWidth,
        80);

    // Alto Pitch Button
    altoPitchToggle.setBounds(
        altoX,
        altoGainSlider.getBottom() + 10,
        sliderWidth,
        25);

    // Alto Section Label
    altoSectionLabel.setBounds(
        altoX,
        altoPitchToggle.getBottom() + 5,
        sliderWidth,
        20);


    // Soprano
    int sopranoX = sopranoSection.getX() + sliderMargin;
    int sopranoY = sopranoSection.getY() + sliderMargin;

    // Soprano Gain
    sopranoGainSlider.setBounds(
        sopranoX,
        sopranoY + 125,
        sliderWidth,
        sliderHeight);

    // Soprano Pan
    sopranoPanSlider.setBounds(
        sopranoX,
        sopranoY + 25,
        sliderWidth,
        80);

    // Soprano Pitch Button
    sopranoPitchToggle.setBounds(
        sopranoX,
        sopranoGainSlider.getBottom() + 10,
        sliderWidth,
        25);

    // Soprano Section Label
    sopranoSectionLabel.setBounds(
        sopranoX,
        sopranoPitchToggle.getBottom() + 5,
        sliderWidth,
        20);
        
    //pan/pitch toggle button (attached to depth component to center at bottom of "chorus area"
    
    const int pitchYOffset = 0; // leftover from a visual misalignment between voices, currently not a problem so ignore

    bassPitchSlider.setBounds(bassPanSlider.getBounds().translated(0, pitchYOffset));
    tenorPitchSlider.setBounds(tenorPanSlider.getBounds().translated(0, pitchYOffset));
    altoPitchSlider.setBounds(altoPanSlider.getBounds().translated(0, pitchYOffset));
    sopranoPitchSlider.setBounds(sopranoPanSlider.getBounds().translated(0, pitchYOffset));

    auto depthBounds = LFOdepth.getBounds();
    const int buttonHeight = 24;
    const int buttonGap    = 8;
    panPitchViewButton.setBounds(depthBounds.getX(),
                                depthBounds.getBottom() + buttonGap,
                                depthBounds.getWidth(),
                                buttonHeight);

}

void LFOTremoloStarterv7AudioProcessorEditor::sliderValueChanged(juce::Slider* slider) //just attaching each param to be updated upon interaction
{
    if (slider == &LFOrate) {
        audioProcessor.LFORateUParam->beginChangeGesture();
        *audioProcessor.LFORateUParam = (float) LFOrate.getValue();
        audioProcessor.LFORateUParam->endChangeGesture();
    }
    else if (slider == &LFOdepth) {
        audioProcessor.LFODepthUParam->beginChangeGesture();
        *audioProcessor.LFODepthUParam = (float) LFOdepth.getValue();
        chorusSection.setColour(juce::Label::backgroundColourId,
                        customLookAndFeel.getDepthMappedBlue((float)LFOdepth.getValue())); //depth knob controls blue hue (customLookAndFeel)
        audioProcessor.LFODepthUParam->endChangeGesture();
    }
    else if (slider == &dryWetSlider) {
        audioProcessor.dryWetParam->beginChangeGesture();
        *audioProcessor.dryWetParam = (float) dryWetSlider.getValue() / 100.0f;
        audioProcessor.dryWetParam->endChangeGesture();
    }
    else if (slider == &bassGainSlider) {
        audioProcessor.bassVoiceGainParam->beginChangeGesture();
        *audioProcessor.bassVoiceGainParam = (float) bassGainSlider.getValue();
        audioProcessor.bassVoiceGainParam->endChangeGesture();
    }
    else if (slider == &bassPanSlider) {
        audioProcessor.bassVoicePanParam->beginChangeGesture();
        *audioProcessor.bassVoicePanParam = (float) bassPanSlider.getValue();
        audioProcessor.bassVoicePanParam->endChangeGesture();
    }
    else if (slider == &tenorGainSlider) {
        audioProcessor.tenorVoiceGainParam->beginChangeGesture();
        *audioProcessor.tenorVoiceGainParam = (float) tenorGainSlider.getValue();
        audioProcessor.tenorVoiceGainParam->endChangeGesture();
    }
    else if (slider == &tenorPanSlider) {
        audioProcessor.tenorVoicePanParam->beginChangeGesture();
        *audioProcessor.tenorVoicePanParam = (float) tenorPanSlider.getValue();
        audioProcessor.tenorVoicePanParam->endChangeGesture();
    }
    else if (slider == &altoGainSlider) {
        audioProcessor.altoVoiceGainParam->beginChangeGesture();
        *audioProcessor.altoVoiceGainParam = (float) altoGainSlider.getValue();
        audioProcessor.altoVoiceGainParam->endChangeGesture();
    }
    else if (slider == &altoPanSlider) {
        audioProcessor.altoVoicePanParam->beginChangeGesture();
        *audioProcessor.altoVoicePanParam = (float) altoPanSlider.getValue();
        audioProcessor.altoVoicePanParam->endChangeGesture();
    }
    else if (slider == &sopranoGainSlider) {
        audioProcessor.sopranoVoiceGainParam->beginChangeGesture();
        *audioProcessor.sopranoVoiceGainParam = (float) sopranoGainSlider.getValue();
        audioProcessor.sopranoVoiceGainParam->endChangeGesture();
    }
    else if (slider == &sopranoPanSlider) {
        audioProcessor.sopranoVoicePanParam->beginChangeGesture();
        *audioProcessor.sopranoVoicePanParam = (float) sopranoPanSlider.getValue();
        audioProcessor.sopranoVoicePanParam->endChangeGesture();
    }
    else if (slider == &bassPitchSlider)
    {
        audioProcessor.bassPitchSemitones->beginChangeGesture();
        *audioProcessor.bassPitchSemitones = (float) bassPitchSlider.getValue();
        audioProcessor.bassPitchSemitones->endChangeGesture();
    }
    else if (slider == &tenorPitchSlider)
    {
        audioProcessor.tenorPitchSemitones->beginChangeGesture();
        *audioProcessor.tenorPitchSemitones = (float) tenorPitchSlider.getValue();
        audioProcessor.tenorPitchSemitones->endChangeGesture();
    }
    else if (slider == &altoPitchSlider)
    {
        audioProcessor.altoPitchSemitones->beginChangeGesture();
        *audioProcessor.altoPitchSemitones = (float) altoPitchSlider.getValue();
        audioProcessor.altoPitchSemitones->endChangeGesture();
    }
    else if (slider == &sopranoPitchSlider)
    {
        audioProcessor.sopranoPitchSemitones->beginChangeGesture();
        *audioProcessor.sopranoPitchSemitones = (float) sopranoPitchSlider.getValue();
        audioProcessor.sopranoPitchSemitones->endChangeGesture();
    }
}

void LFOTremoloStarterv7AudioProcessorEditor::updatePanPitchView() //function for label changing between pan/pitch views, resets GUI, just handles the bools really
{
    const bool showPitch = showPitchView;

    bassPanSlider.setVisible(!showPitch);
    tenorPanSlider.setVisible(!showPitch);
    altoPanSlider.setVisible(!showPitch);
    sopranoPanSlider.setVisible(!showPitch);

    bassPitchSlider.setVisible(showPitch);
    tenorPitchSlider.setVisible(showPitch);
    altoPitchSlider.setVisible(showPitch);
    sopranoPitchSlider.setVisible(showPitch);

    bassPanLabel.setVisible(true);
    tenorPanLabel.setVisible(true);
    altoPanLabel.setVisible(true);
    sopranoPanLabel.setVisible(true);

    bassPanLabel.setText(showPitch ? "Pitch" : "Pan", juce::dontSendNotification);
    tenorPanLabel.setText(showPitch ? "Pitch" : "Pan", juce::dontSendNotification);
    altoPanLabel.setText(showPitch ? "Pitch" : "Pan", juce::dontSendNotification);
    sopranoPanLabel.setText(showPitch ? "Pitch" : "Pan", juce::dontSendNotification);

    panPitchViewButton.setButtonText(showPitch ? "Pan" : "Pitch");
    
}

void LFOTremoloStarterv7AudioProcessorEditor::buttonClicked(juce::Button* button) //button toggle behavior
{
    if (button == &panPitchViewButton)
    {
        showPitchView = panPitchViewButton.getToggleState();
        updatePanPitchView();
        resized();
        repaint(); 
        return;
    }
    
    if (button == &bassPitchToggle && audioProcessor.bassPitchEnabledParam)
    {
        audioProcessor.bassPitchEnabledParam->beginChangeGesture();
        audioProcessor.bassPitchEnabledParam->setValueNotifyingHost(bassPitchToggle.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.bassPitchEnabledParam->endChangeGesture();
    }
    else if (button == &tenorPitchToggle && audioProcessor.tenorPitchEnabledParam)
    {
        audioProcessor.tenorPitchEnabledParam->beginChangeGesture();
        audioProcessor.tenorPitchEnabledParam->setValueNotifyingHost(tenorPitchToggle.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.tenorPitchEnabledParam->endChangeGesture();
    }
    else if (button == &altoPitchToggle && audioProcessor.altoPitchEnabledParam)
    {
        audioProcessor.altoPitchEnabledParam->beginChangeGesture();
        audioProcessor.altoPitchEnabledParam->setValueNotifyingHost(altoPitchToggle.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.altoPitchEnabledParam->endChangeGesture();
    }
    else if (button == &sopranoPitchToggle && audioProcessor.sopranoPitchEnabledParam)
    {
        audioProcessor.sopranoPitchEnabledParam->beginChangeGesture();
        audioProcessor.sopranoPitchEnabledParam->setValueNotifyingHost(sopranoPitchToggle.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.sopranoPitchEnabledParam->endChangeGesture();
    }
    
    if (button == &bassSectionLabel)
        audioProcessor.setBassMuted(bassSectionLabel.getToggleState());
    else if (button == &tenorSectionLabel)
        audioProcessor.setTenorMuted(tenorSectionLabel.getToggleState());
    else if (button == &altoSectionLabel)
        audioProcessor.setAltoMuted(altoSectionLabel.getToggleState());
    else if (button == &sopranoSectionLabel)
        audioProcessor.setSopranoMuted(sopranoSectionLabel.getToggleState());
    
    if (button == &bassSectionLabel && audioProcessor.bassMuteParam)
    {
        audioProcessor.bassMuteParam->beginChangeGesture();
        audioProcessor.bassMuteParam->setValueNotifyingHost(bassSectionLabel.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.bassMuteParam->endChangeGesture();
    }
    if (button == &tenorSectionLabel && audioProcessor.tenorMuteParam)
    {
        audioProcessor.tenorMuteParam->beginChangeGesture();
        audioProcessor.tenorMuteParam->setValueNotifyingHost(tenorSectionLabel.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.tenorMuteParam->endChangeGesture();
    }
    if (button == &altoSectionLabel && audioProcessor.altoMuteParam)
    {
        audioProcessor.altoMuteParam->beginChangeGesture();
        audioProcessor.altoMuteParam->setValueNotifyingHost(altoSectionLabel.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.altoMuteParam->endChangeGesture();
    }
    if (button == &sopranoSectionLabel && audioProcessor.sopranoMuteParam)
    {
        audioProcessor.sopranoMuteParam->beginChangeGesture();
        audioProcessor.sopranoMuteParam->setValueNotifyingHost(sopranoSectionLabel.getToggleState() ? 1.0f : 0.0f);
        audioProcessor.sopranoMuteParam->endChangeGesture();
    }
    
    refreshMuteUI();

}

void LFOTremoloStarterv7AudioProcessorEditor::timerCallback() //update slider values when automated DAW side
{   
    auto syncSlider = [](juce::Slider& s, float v) //quick function to make sure it doesn't interfere with user interaction
    {
        if (s.isMouseButtonDown())
            return;

        if ((float)s.getValue() != v)
            s.setValue(v, juce::dontSendNotification);
    };
    
    //gain sync
     if (audioProcessor.bassVoiceGainParam)
        syncSlider(bassGainSlider, audioProcessor.bassVoiceGainParam->get());

    if (audioProcessor.tenorVoiceGainParam)
        syncSlider(tenorGainSlider, audioProcessor.tenorVoiceGainParam->get());

    if (audioProcessor.altoVoiceGainParam)
        syncSlider(altoGainSlider, audioProcessor.altoVoiceGainParam->get());

    if (audioProcessor.sopranoVoiceGainParam)
        syncSlider(sopranoGainSlider, audioProcessor.sopranoVoiceGainParam->get());
        
    //pan sync
    if (audioProcessor.bassVoicePanParam)
        syncSlider(bassPanSlider, audioProcessor.bassVoicePanParam->get());

    if (audioProcessor.tenorVoicePanParam)
        syncSlider(tenorPanSlider, audioProcessor.tenorVoicePanParam->get());

    if (audioProcessor.altoVoicePanParam)
        syncSlider(altoPanSlider, audioProcessor.altoVoicePanParam->get());

    if (audioProcessor.sopranoVoicePanParam)
        syncSlider(sopranoPanSlider, audioProcessor.sopranoVoicePanParam->get());
    
    //pitch sync
    if (audioProcessor.bassPitchSemitones)
        syncSlider(bassPitchSlider,   audioProcessor.bassPitchSemitones->get());
        
    if (audioProcessor.tenorPitchSemitones)
        syncSlider(tenorPitchSlider,  audioProcessor.tenorPitchSemitones->get());
        
    if (audioProcessor.altoPitchSemitones)
        syncSlider(altoPitchSlider,   audioProcessor.altoPitchSemitones->get());
        
    if (audioProcessor.sopranoPitchSemitones)
        syncSlider(sopranoPitchSlider,audioProcessor.sopranoPitchSemitones->get());
        
    //pitch toggle
    if (audioProcessor.bassPitchEnabledParam)
        bassPitchToggle.setToggleState(audioProcessor.bassPitchEnabledParam->get(), juce::dontSendNotification);

    if (audioProcessor.tenorPitchEnabledParam)
        tenorPitchToggle.setToggleState(audioProcessor.tenorPitchEnabledParam->get(), juce::dontSendNotification);

    if (audioProcessor.altoPitchEnabledParam)
        altoPitchToggle.setToggleState(audioProcessor.altoPitchEnabledParam->get(), juce::dontSendNotification);

    if (audioProcessor.sopranoPitchEnabledParam)
        sopranoPitchToggle.setToggleState(audioProcessor.sopranoPitchEnabledParam->get(), juce::dontSendNotification);

    // Chorus param sync
    if (audioProcessor.LFORateUParam)
        syncSlider(LFOrate, (float)audioProcessor.LFORateUParam->get());
        
    if (audioProcessor.LFODepthUParam)
        syncSlider(LFOdepth,(float)audioProcessor.LFODepthUParam->get());
        
    if (audioProcessor.dryWetParam)
        syncSlider(dryWetSlider, (float)audioProcessor.dryWetParam->get() * 100.0f);
        
    //mute sync
    if (audioProcessor.bassMuteParam)
        bassSectionLabel.setToggleState(audioProcessor.bassMuteParam->get(), juce::dontSendNotification);

    if (audioProcessor.tenorMuteParam)
        tenorSectionLabel.setToggleState(audioProcessor.tenorMuteParam->get(), juce::dontSendNotification);

    if (audioProcessor.altoMuteParam)
        altoSectionLabel.setToggleState(audioProcessor.altoMuteParam->get(), juce::dontSendNotification);

    if (audioProcessor.sopranoMuteParam)
        sopranoSectionLabel.setToggleState(audioProcessor.sopranoMuteParam->get(), juce::dontSendNotification);
    
    refreshMuteUI();
}
void LFOTremoloStarterv7AudioProcessorEditor::refreshMuteUI() //basic custom function to set visual updates for when mute is toggled (greying out voice params)
{
    const bool bassMuted    = audioProcessor.getBassMuted();
    const bool tenorMuted   = audioProcessor.getTenorMuted();
    const bool altoMuted    = audioProcessor.getAltoMuted();
    const bool sopranoMuted = audioProcessor.getSopranoMuted();

    // match state
    bassSectionLabel.setToggleState(bassMuted, juce::dontSendNotification);
    tenorSectionLabel.setToggleState(tenorMuted, juce::dontSendNotification);
    altoSectionLabel.setToggleState(altoMuted, juce::dontSendNotification);
    sopranoSectionLabel.setToggleState(sopranoMuted, juce::dontSendNotification);

    // set muted
    bassGainSlider.setEnabled(!bassMuted);
    bassGainLabel.setEnabled(!bassMuted);
    bassPanSlider.setEnabled(!bassMuted);
    bassPitchSlider.setEnabled(!bassMuted);
    bassPitchToggle.setEnabled(!bassMuted);

    tenorGainSlider.setEnabled(!tenorMuted);
    tenorGainLabel.setEnabled(!tenorMuted);
    tenorPanSlider.setEnabled(!tenorMuted);
    tenorPitchSlider.setEnabled(!tenorMuted);
    tenorPitchToggle.setEnabled(!tenorMuted);

    altoGainSlider.setEnabled(!altoMuted);
    altoGainLabel.setEnabled(!altoMuted);
    altoPanSlider.setEnabled(!altoMuted);
    altoPitchSlider.setEnabled(!altoMuted);
    altoPitchToggle.setEnabled(!altoMuted);

    sopranoGainSlider.setEnabled(!sopranoMuted);
    sopranoGainLabel.setEnabled(!sopranoMuted);
    sopranoPanSlider.setEnabled(!sopranoMuted);
    sopranoPitchSlider.setEnabled(!sopranoMuted);
    sopranoPitchToggle.setEnabled(!sopranoMuted);
    
    // set grey
    bassGainLabel.setAlpha(bassMuted ? 0.35f : 1.0f);
    tenorGainLabel.setAlpha(tenorMuted ? 0.35f : 1.0f);
    altoGainLabel.setAlpha(altoMuted ? 0.35f : 1.0f);
    sopranoGainLabel.setAlpha(sopranoMuted ? 0.35f : 1.0f);
    
    bassPanLabel.setAlpha(bassMuted ? 0.35f : 1.0f);
    tenorPanLabel.setAlpha(tenorMuted ? 0.35f : 1.0f);
    altoPanLabel.setAlpha(altoMuted ? 0.35f : 1.0f);
    sopranoPanLabel.setAlpha(sopranoMuted ? 0.35f : 1.0f);
    
    bassSectionLabel.setAlpha(bassMuted ? 0.35f : 1.0f);
    tenorSectionLabel.setAlpha(tenorMuted ? 0.35f : 1.0f);
    altoSectionLabel.setAlpha(altoMuted ? 0.35f : 1.0f);
    sopranoSectionLabel.setAlpha(sopranoMuted ? 0.35f : 1.0f);
    
    //still figuring out how to change color of textboxes attached to slider components, i.e. gain label and pan label
    
    repaint();
}