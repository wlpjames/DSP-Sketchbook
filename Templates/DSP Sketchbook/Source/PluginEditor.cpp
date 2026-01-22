/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DSPSketchbookAudioProcessorEditor::DSPSketchbookAudioProcessorEditor (DSPSketchbookAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, mainPanel(p)
{
    setResizable(true, false);
    setSize (500, 750);
    setLookAndFeel(&p.lookAndFeel);
    addAndMakeVisible(mainPanel);
}

DSPSketchbookAudioProcessorEditor::~DSPSketchbookAudioProcessorEditor()
{
}

//==============================================================================
void DSPSketchbookAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DSPSketchbookAudioProcessorEditor::resized()
{
    mainPanel.setBounds(getLocalBounds());
}

sketchbook::Module* DSPSketchbookAudioProcessorEditor::getLatestPlayingModuleByName(juce::String name)
{
    return audioProcessor.audioEngine.getLatestPlayingModuleByName(name);
}
