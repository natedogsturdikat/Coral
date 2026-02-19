/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 3 Jun 2025 3:45:20pm
    Author:  Nathan Sturdivant

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Choral Pink slider thumbs
        setColour(juce::Slider::thumbColourId, juce::Colour(0xfff88379));
        
    }
    
    //ocean background color shift: makes blue darker as depth is increased
    juce::Colour CustomLookAndFeel::getDepthMappedBlue(float depthValue) const
    {
        const int minBlue = 160;  // high depth (originally 160)
        const int maxBlue = 190;  //low depth (originally 240)

        //higher depth → darker blue
        float invertedDepth = 100.0f - juce::jlimit(0.0f, 100.0f, depthValue);

        int blueVal = juce::jlimit(minBlue, maxBlue,
            static_cast<int>(minBlue + (invertedDepth / 100.0f) * (maxBlue - minBlue)));

        int red   = juce::jmap(blueVal, maxBlue, minBlue, 70, 35); // originally 80, 35
        int green = juce::jmap(blueVal, maxBlue, minBlue, 190, 115); //originally 220, 115)

        return juce::Colour(red, green, blueVal);
    }
    
    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override
    {
    // erases textbox outline (intentionally blank)
    }
    
    //Visual aid: creates a highlight over interactable buttons when hovered over with mouse
    void drawButtonBackground(juce::Graphics& g,juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto r = button.getLocalBounds().toFloat().reduced(2.0f);
        auto fill = juce::Colours::white.withAlpha(0.12f);

        if (isMouseOverButton)
        {
            fill = fill.withAlpha(0.18f);
            g.setColour(fill);
            g.fillRoundedRectangle(r, 7.0f);
        }
        if (isButtonDown)      fill = fill.withAlpha(0.22f);

    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto r = label.getLocalBounds();

        auto bg = label.findColour(juce::Label::backgroundColourId);
        if (bg.getAlpha() > 0)
        {
            g.setColour(bg);
            g.fillRect(r);
        }

        drawOutlinedText(g,label.getText(),r,getLabelFont(label),label.getJustificationType());
    }
    
    void drawOutlinedText(juce::Graphics& g, const juce::String& text, juce::Rectangle<int> bounds, const juce::Font& font, juce::Justification just)
    {
        g.setFont(font);

        // outline colour
        juce::Colour outline = juce::Colours::black.withAlpha(0.4f);

        g.setColour(outline);

        // sub-pixel outline (stacking multiple smaller outline pixels of lighter hues to create a less stark outline color)
        g.drawFittedText(text, bounds.translated( 1, 0), just, 1);
        g.drawFittedText(text, bounds.translated(-1, 0), just, 1);
        g.drawFittedText(text, bounds.translated( 0, 1), just, 1);
        g.drawFittedText(text, bounds.translated( 0,-1), just, 1);

        // main text
        g.setColour(juce::Colours::white);
        g.drawFittedText(text, bounds, just, 1);
    }
    
    void drawButtonText(juce::Graphics& g,juce::TextButton& button,bool, bool) override
    {
        drawOutlinedText(g,button.getButtonText(),button.getLocalBounds(),juce::Font(15.0f, juce::Font::bold),juce::Justification::centred);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        float size = 14.0f; //default 14
        //sets the font size based on each label (like subheader vs body text in html)
        if (label.getText() == "Bass" || label.getText() == "Tenor" || label.getText() == "Alto" || label.getText() == "Soprano")
        {
            size = 18.0f;
        }

        else if (label.getText() == "Gain" || label.getText() == "Pan"  || label.getText() == "Pitch")
        {
            size = 16.0f;
        }

        return juce::Font(size, juce::Font::bold);
    }
};
