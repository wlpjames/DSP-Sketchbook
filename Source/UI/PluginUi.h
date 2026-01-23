/*
  ==============================================================================

    PluginUi.h
    Created: 27 Apr 2025 8:06:57pm
    Author:  William James

  ==============================================================================
*/

#pragma once
#include "../../Templates/DSP Sketchbook/Source/PluginProcessor.h"
#include "ParamaterPages.h"

namespace sketchbook
{
//==============================================================================
class ScopeComponent  : public juce::Component,
private juce::Timer
{
    public:
    using Queue = AudioBufferQueue;
    
    enum scopeToShow
    {
        osc, freq
    };
    
    //==============================================================================
    ScopeComponent (Queue& queueToUse)
    : audioBufferQueue (queueToUse)
    {
        sampleData.fill (0.f);
        setFramesPerSecond (30);
        
        addAndMakeVisible(scopeSwitchButton);
        scopeSwitchButton.setButtonText("Toggle");
        scopeSwitchButton.setClickingTogglesState(true);
        scopeSwitchButton.onClick = [sp = SafePointer<ScopeComponent>(this)] ()
        {
            if (!sp) return;
            
            sp->showScope(sp->scopeSwitchButton.getToggleState() ? scopeToShow::osc : scopeToShow::freq);
        };
        
        showScope(freq);
    }
    
    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
    }
    
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        auto area = getLocalBounds();
        
        // Spectrum
        auto scopeRect = area.toFloat();
        g.setColour({25, 25, 25});
        g.fillRoundedRectangle(scopeRect, 5);
        
        scopeRect.reduce(10, 10);
        
        switch (currScope) {
                
            case osc:
            {
                g.setColour ({65, 65, 65});
                plot (sampleData.data(), sampleData.size(), g, scopeRect, 1.f, scopeRect.getHeight() / 2);
                break;
            }
                
            case freq:
                g.setColour ({65, 65, 65});
                plot (spectrumData.data(), spectrumData.size() / 4, g, scopeRect.reduced(10));
                break;
                
            default:
                break;
        }
    }
    
    void showScope(scopeToShow scope)
    {
        currScope = scope;
    }
    
    //==============================================================================
    void resized() override
    {
        auto area = getLocalBounds().reduced(5);
        scopeSwitchButton.setBounds(juce::Rectangle<int>(60, 24).withRightX(area.getRight()).withY(area.getY()));
    }
    
    private:
    
    //==============================================================================
    void timerCallback() override
    {
        audioBufferQueue.pop (sampleData.data());
        juce::FloatVectorOperations::copy (spectrumData.data(), sampleData.data(), (int) sampleData.size());
        
        auto fftSize = (size_t) fft.getSize();
        
        jassert (spectrumData.size() == 2 * fftSize);
        windowFun.multiplyWithWindowingTable (spectrumData.data(), fftSize);
        fft.performFrequencyOnlyForwardTransform (spectrumData.data());
        
        static constexpr auto mindB = -160.f;
        static constexpr auto maxdB = 0.f;
        
        for (auto& s : spectrumData)
            s = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (s) - juce::Decibels::gainToDecibels (float(fftSize))), mindB, maxdB, 0.f, 1.f);
        
        repaint();
    }
    
    //==============================================================================
    static void plot (const float* data,
                      size_t numSamples,
                      juce::Graphics& g,
                      juce::Rectangle<float> rect,
                      float scaler = 1.f,
                      float offset = 0.f)
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight();
        
        auto center = rect.getBottom() - offset;
        auto gain = h * scaler;
        
        for (size_t i = 1; i < numSamples; ++i)
            g.drawLine ({ juce::jmap (float(i - 1), 0.f, float(numSamples - 1), float(right - w), float(right)),
                          center - gain * data[i - 1],
                          juce::jmap (float(i), 0.f, float(numSamples - 1), float(right - w), float(right)),
                          center - gain * data[i] });
    }
    
    private:
    //==============================================================================
    scopeToShow currScope = osc;
    juce::TextButton scopeSwitchButton;
    
    Queue& audioBufferQueue;
    std::array<float, Queue::bufferSize> sampleData;
    
    juce::dsp::FFT fft { Queue::order };
    using WindowFun = juce::dsp::WindowingFunction<float>;
    WindowFun windowFun { (size_t) fft.getSize(), WindowFun::hann };
    std::array<float, 2 * Queue::bufferSize> spectrumData;
};

class HeaderComponent : public juce::Component
{
    public:
    HeaderComponent()
    {
        addAndMakeVisible(titleLabel);
        titleLabel.setFont(juce::FontOptions("Andale Mono", 21, juce::Font::bold));
        titleLabel.setColour(juce::Label::ColourIds::textColourId, {220, 220, 220});
        titleLabel.setJustificationType(juce::Justification::centred);
        titleLabel.setText("Physical Modeling Sketchbook", juce::dontSendNotification);
    }
    
    void resized() override
    {
        titleLabel.setBounds(getLocalBounds());
    }
    
    private:
    juce::Label titleLabel;
};

class KeyboardComponent : public juce::MidiKeyboardComponent
{
    public:
    KeyboardComponent(juce::MidiKeyboardState& state, Orientation orientation)
    : MidiKeyboardComponent(state, orientation)
    {
        setMidiChannel (1);
        setAvailableRange(48, 91);
        setColour(juce::MidiKeyboardComponent::ColourIds::blackNoteColourId, {28, 28, 28});
        setColour(juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId, {220, 220, 220});
    }
    
    void drawBlackNote(int /*midiNoteNumber*/, juce::Graphics& g, juce::Rectangle<float> area,
                       bool isDown, bool isOver, juce::Colour noteFillColour) override
    {
        juce::Colour colour = isDown ? juce::Colour(35, 35, 35) : juce::Colour(28, 28, 28);
        g.setColour(colour);
        g.fillRect(area);
    }
    
    /*
     void drawWhiteNote(int midiNoteNumber, Graphics& g, Rectangle<float> area,
     bool isDown, bool isOver, Colour lineColour, Colour textColour) override
     */
};

class PageMenu : public juce::Component
{
    class MenuButton : public juce::TextButton
    {
        void paint(juce::Graphics& g) override
        {
            //just draw text
            g.setColour(getToggleState() ? juce::Colour(220, 220, 220) : juce::Colour(60, 60, 60));
            g.setFont(juce::Font(juce::FontOptions("Andale Mono", 17, juce::Font::FontStyleFlags::plain)));
            g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred);
        }
    };
    
    public:
    ~PageMenu()
    {
        for (auto& b : buttons)
            if (b)
                delete b;
    }
    
    void resized()
    {
        auto area = getLocalBounds();
        
        for (auto& b : buttons)
            b->setBounds(area.removeFromLeft(getWidth() / buttons.size()));
    }
    
    void addOptions(juce::StringArray options)
    {
        //not tested for run time changes
        jassert(buttons.size() == 0);
        
        for (int i = 0; i < options.size(); i++)
        {
            auto* button = new MenuButton();
            addAndMakeVisible(button);
            button->setButtonText(options[i]);
            button->setRadioGroupId(1);
            button->setClickingTogglesState(true);
            button->onClick = [sp = SafePointer<PageMenu>(this), i] ()
            {
                if (!sp) return;
                
                if (sp->onSelectionFunc && sp->buttons[i]->getToggleState())
                    sp->onSelectionFunc(i);
            };
            buttons.add(button);
        }
    }
    
    void select(int index)
    {
        if (index >= 0 && index < buttons.size())
        {
            buttons[index]->setToggleState(true, juce::sendNotification);
        }
    }
    
    std::function<void(int)> onSelectionFunc;
    
    private:
    juce::Array<MenuButton*> buttons;
};

class MainPanelComponent : public juce::Component
{
    public:
    MainPanelComponent(sketchbook::Context& _context)
    : keyboardComponent(midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    , scopeComponent(*_context.audioBufferQueue)
    , context(_context)
    {
        addAndMakeVisible(keyboardComponent);
        addAndMakeVisible(scopeComponent);
        
        midiKeyboardState.addListener(&context.midiMessageCollector);
        
        addAndMakeVisible(pages);
        pages.setData(context.parameterData);
        
        addAndMakeVisible(pageMenu);
        pageMenu.onSelectionFunc = [sp = SafePointer<MainPanelComponent>(this)] (int index)
        {
            if (!sp)
                return;
            
            sp->pages.showPage(index);
        };
        pageMenu.addOptions({"PARAMETERS", "MAPPINGS", "MODULATION SOURCES", "EFFECTS"});
        pageMenu.select(0);
        
        addAndMakeVisible(header);
    }
    
    ~MainPanelComponent()
    {
        midiKeyboardState.removeListener(&context.midiMessageCollector);
    }
    
    void resized()
    {
        auto area = getLocalBounds();
        header.setBounds(area.removeFromTop(80));
        keyboardComponent.setBounds (area.removeFromTop(100).reduced(10, 10));
        area.removeFromTop(10);
        scopeComponent.setBounds(area.removeFromTop(140).reduced(10, 20));
        pageMenu.setBounds(area.removeFromTop(40));
        
        pages.setBounds(area.reduced(10, 10));
        
        keyboardComponent.setKeyWidth(18);
    }
    
    void paint(juce::Graphics& g)
    {
        g.fillAll ({25, 25, 25});
        
        g.setColour({30, 30, 30});
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10);
        
        //darker for around the keyboard
        g.setColour({25, 25, 25});
        g.fillRect(keyboardComponent.getBoundsInParent().expanded(10, 10));
    }
    
    private:
    juce::MidiKeyboardState midiKeyboardState;
    KeyboardComponent keyboardComponent;
    ScopeComponent scopeComponent;
    sketchbook::Pages pages;
    PageMenu pageMenu;
    HeaderComponent header;
    sketchbook::Context& context;
};

} //end namespace sketchbook
