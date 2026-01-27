/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DSPSketchbookAudioProcessorEditor::DSPSketchbookAudioProcessorEditor (juce::AudioProcessor& p,
                                                                      sketchbook::Context& ctx,
                                                                      sketchbook::AppLookAndFeel& laf)
: AudioProcessorEditor(&p)
, audioProcessor(p)
, mainPanel(ctx)
{
    setResizable(true, false);
    setSize(500, 750);
    setLookAndFeel(&laf);
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
