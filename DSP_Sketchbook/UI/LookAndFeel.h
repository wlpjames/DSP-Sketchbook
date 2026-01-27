/*
  ==============================================================================

    LookAndFeel.h
    Created: 16 May 2025 12:19:10pm
    Author:  William James

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "StyledSlider.h"

namespace sketchbook
{
class AppLookAndFeel : public juce::LookAndFeel_V4
{
    public:
    
    AppLookAndFeel()
    {
        setDefaultSansSerifTypefaceName("Andale Mono");
        
        setColour(juce::PopupMenu::backgroundColourId,  juce::Colours::white.withAlpha(0.0f));
        
        setColour(juce::TextButton::textColourOnId,     juce::Colours::white);
        setColour(juce::TextButton::textColourOffId,    juce::Colours::whitesmoke);
        
        setColour(juce::ScrollBar::ColourIds::thumbColourId, {60, 60, 60});
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto ss = dynamic_cast<StyledSlider*>( &slider);
        
        if (!ss)
        {
            return;
        }
        
        //ColourPalette::ColourVarient p = ss->getColours();
        
        //define squared area to the right of the space
        auto area = juce::Rectangle<float>(x, y, width, height);
        auto knobArea = juce::Rectangle<float>(juce::jmin(width, height), juce::jmin(width, height)).withCentre(area.getCentre());
        
        //draw large circle in darkest
        g.setColour(juce::Colours::black);
        g.fillEllipse(knobArea);
        
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto trackStartRadians = rotaryStartAngle;
        auto knobSettingRadians = toAngle;
        auto middlePosAngle = M_2_PI;
        
        bool symmetric = false;
        if (symmetric)
        {
            if (toAngle < middlePosAngle)
            {
                // highlight left portion
                trackStartRadians = knobSettingRadians;
                knobSettingRadians = middlePosAngle;
            }
            else
            {
                // highlight right portion
                trackStartRadians = middlePosAngle;
            }
        }
        
        auto kaRed = knobArea.reduced(2);
        
        //fill segment in light
        g.setColour( juce::Colours::whitesmoke);
        
        juce::Path path;
        path.addCentredArc(kaRed.getCentreX(), kaRed.getCentreY(), kaRed.getWidth() / 2, kaRed.getHeight() / 2, 0.0, trackStartRadians, knobSettingRadians, true);
        g.strokePath(path, juce::PathStrokeType(4, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
        
        //draw the modulation amount semi transparent above
        if (ss->shouldDisplayModulation())
        {
            float modStartRadians = knobSettingRadians;
            float modToRadians = juce::jmin(modStartRadians + (rotaryEndAngle - rotaryStartAngle) * ss->getModulatedValue(), rotaryEndAngle);
            
            g.setColour(juce::Colours::grey.withAlpha(0.8f));
            
            juce::Path modPath;
            path.addCentredArc(kaRed.getCentreX(), kaRed.getCentreY(), kaRed.getWidth() / 2, kaRed.getHeight() / 2, 0.0, modStartRadians, modToRadians, true);
            g.strokePath(path, juce::PathStrokeType(4, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
        }
        
        //draw middle in darkest
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(knobArea.reduced( 3 ));
        
        //if mouseOver or isDragging then draw text
        if (ss->isMouseOverOrDragging())
        {
            g.setFont(10);
            g.setColour(juce::Colours::black);
            g.drawText(juce::String::toDecimalStringWithSignificantFigures(ss->getValue(), 2),
                       knobArea.reduced(5), juce::Justification::centred);
        }
    }
    
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        
        g.fillAll (slider.findColour (juce::Slider::backgroundColourId));
        
        if (style != juce::Slider::LinearBar)
        {
            juce::LookAndFeel_V3::drawLinearSlider(g, x, y, width, height,
                                                   sliderPos, minSliderPos,
                                                   maxSliderPos, style, slider);
        }
        
        auto ss = dynamic_cast<StyledLinearSlider*>(&slider);
        
        if (!ss)
        {
            return;
        }
        
        const float fx = (float) x, fy = (float) y, fw = (float) width, fh = (float) height;
        
        juce::Path p;
        p.addRectangle(fx, fy, sliderPos - fx, fh);
        
        auto baseColour = slider.findColour (juce::Slider::thumbColourId)
            .withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f)
            .withMultipliedAlpha (0.8f);
        
        g.setGradientFill (juce::ColourGradient::horizontal(baseColour.brighter (0.08f), 0.0f,
                                                            baseColour.darker (0.4f), (float) height));
        g.fillPath (p);
        
        //draw a thumb?
        g.setColour (baseColour.darker (0.2f));
        g.fillRect (sliderPos, fy, 2.5f, fh);
        
        drawLinearSliderOutline (g, x, y, width, height, style, slider);
        
        //draw the modulation amount semi transparent above
        if (ss->shouldDisplayModulation())
        {
            float modValue = ss->getModulatedValue();
            float modulatedSliderPos = width * modValue;
            g.setColour(baseColour.darker (0.6f));
            g.fillRect (fx, fy, modulatedSliderPos, fh / 5);
        }
        
    }
    
    //MARK: POPUP Menu (right click)
    void drawPopupMenuBackground (juce::Graphics &g, int width, int height) override
    {
        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(juce::Rectangle<int>(width, height).toFloat(), 6.0);
    }
    
    void drawPopupMenuItem (juce::Graphics &g,  const juce::Rectangle< int > &area,
                            bool isSeparator,   bool isActive,
                            bool isHighlighted, bool isTicked,
                            bool hasSubMenu,    const juce::String &text,
                            const juce::String &shortcutKeyText,
                            const juce::Drawable *icon,
                            const juce::Colour *textColour) override
    {
        if (isSeparator)
        {
            auto a = area.reduced(5);
            g.setColour(juce::Colours::black);
            g.drawLine(a.getX(), a.getHeight() / 2, a.getWidth(), a.getHeight() / 2, 2.0);
        }
        else
        {
            if (isHighlighted)
            {
                g.fillAll(juce::Colours::lightgrey);
            }
            g.drawText(text, area, juce::Justification::centred);
        }
    }
    
    int getPopupMenuBorderSize() override
    {
        return 0;
    }
    
    void preparePopupMenuWindow(juce::Component& c) override
    {
        c.setOpaque(false);
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour &backgroundColour,
                              bool  shouldDrawButtonAsHighlighted,
                              bool  shouldDrawButtonAsDown) override
    {
        auto area = button.getLocalBounds().toFloat();
        juce::Colour grey = {55, 55, 55};
        g.setColour(button.getToggleState() ? grey.brighter() : grey);
        g.drawRoundedRectangle(area.reduced(1.0), area.getHeight() / 2, 1.5);
    }
    
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(juce::FontOptions("Andale Mono", 12, juce::Font::bold));
    }
    
    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& comboBox) override
    {
        auto area = comboBox.getLocalBounds().toFloat();
        juce::Colour grey = {55, 55, 55};
        g.setColour(grey);
        g.drawRoundedRectangle(area.reduced(1.0), area.getHeight() / 2, 1.5);
        
        juce::Rectangle<int> arrowZone (width - 25, 0, 18, height);
        juce::Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        
        g.setColour(grey.withAlpha((comboBox.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType (2.0f));
    }
    
    juce::Font getComboBoxFont (juce::ComboBox&) override
    {
        return juce::Font(juce::FontOptions("Andale Mono", 12, juce::Font::bold));
    }
};

} //end namespace sketchbook
