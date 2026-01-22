/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../Source/DSP_Sketchbook.h"

//==============================================================================
/**
*/
class DSPSketchbookAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DSPSketchbookAudioProcessor();
    ~DSPSketchbookAudioProcessor() override;

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
    
    juce::MidiMessageCollector& getMidiMessageCollector() noexcept { return midiMessageCollector; }
    sketchbook::AudioBufferQueue<float>& getAudioBufferQueue() noexcept { return audioBufferQueue; }

    sketchbook::AudioEngine<sketchbook::ModuleList<SimpleOsc>, sketchbook::ModuleList<Convolution/*, Delay<float>, Distortion*/>> audioEngine;
    
private:
    //==============================================================================
    juce::MidiMessageCollector midiMessageCollector;
    sketchbook::AudioBufferQueue<float> audioBufferQueue;
    sketchbook::ScopeDataCollector<float> scopeDataCollector { audioBufferQueue };
    sketchbook::AppLookAndFeel lookAndFeel;

    friend class DSPSketchbookAudioProcessorEditor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DSPSketchbookAudioProcessor)
};
