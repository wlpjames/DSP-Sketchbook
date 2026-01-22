/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../../Source/DSP_Sketchbook.h"
#include "../../../Source/UI/PluginUi.h"

//==============================================================================
/**
*/
class DSPSketchbookAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DSPSketchbookAudioProcessorEditor (DSPSketchbookAudioProcessor&);
    ~DSPSketchbookAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    sketchbook::Module* getLatestPlayingModuleByName(juce::String name);
    
private:
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DSPSketchbookAudioProcessor& audioProcessor;
    sketchbook::MainPanelComponent mainPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DSPSketchbookAudioProcessorEditor)
};
