/*
  ==============================================================================

    ModulationSources.h
    Created: 16 May 2025 12:18:30pm
    Author:  William James

  ==============================================================================
*/

#pragma once
#include "../Engine/Module.h"

class LfoModule : public sketchbook::Module
{
public:
    
    LfoModule()
    : frequency(0.5f)
    , depth(1.0f)
    , phase(0.0f)
    , sampleRate(44100.0f)
    {
        setModuleParameters(
        {
            Parameter::Float("Rate Hz", [this](juce::var value)
            {
                frequency = static_cast<float>(value);
                updatePhaseIncrement();
            }, 0.5, 0.01f, 10.0),

            Parameter::Float("Depth", [this](juce::var value)
            {
                depth = static_cast<float>(value);
            }, 1.0, 0.0f, 1.0)
        });
    }

    virtual ~LfoModule() {}

    void prepareToPlay(float samplerate, int buffersize) override {
        sampleRate = samplerate;
        updatePhaseIncrement();
    }

    void noteOn(const sketchbook::NoteOnEvent& event) override {
        phase = 0.0f;
    }

    void noteOff(bool) override
    {
    }

    void reset() override {
        phase = 0.0f;
    }

    void applyMidi(const juce::MidiMessage& message) override
    {
    }

    void processSample(float* sample) override
    {
        float lfoValue = depth * std::sin(phase);
        *sample *= (1.f + lfoValue) / 2.f;  // Amplitude modulation
        
        phase += phaseIncrement;
        if (phase >= 2.0f * M_PI)
            phase -= 2.0f * M_PI;
        
        internalBuffer.appendSingleSample(*sample);
    }

    juce::String getName() override
    {
        return "LFO";
    }

private:
    
    void updatePhaseIncrement()
    {
        phaseIncrement = 2.0f * M_PI * (frequency / sampleRate);
    }

    float frequency;
    float depth;
    float phase;
    float phaseIncrement;
    float sampleRate;
};

/*
class ADSRModule : public Module {
public:
    ADSRModule()
    : sampleRate(44100.0f)
    , envelopeTime(0.0f)
    , currentLevel(0.0f)
    , instanceId(_instanceId)
    , state(State::Idle)
    {
        setModuleParameters({
            
            Parameter::Float("Attack", [this](var value)
            {
                attackTime = static_cast<float>(value);
                attackSamples = attackTime * sampleRate;
            }, 0.01f, 0.001f, 10.0f),

            Parameter::Float("Decay", [this](var value)
            {
                decayTime = static_cast<float>(value);
                decaySamples = decayTime * sampleRate;
            }, 0.1f, 0.001f, 10.0f),

            Parameter::Float("Sustain", [this](var value)
            {
                sustainLevel = static_cast<float>(value);
            }, 0.8f, 0.0f, 1.0f),

            Parameter::Float("Release", [this](var value)
            {
                releaseTime = static_cast<float>(value);
                releaseSamples = releaseTime * sampleRate;
            }, 0.5f, 0.001f, 10.0f),
        });
    }

    virtual ~ADSRModule() {}

    void prepareToPlay(float samplerate, int buffersize) override {
        sampleRate = samplerate;
        attackSamples  = attackTime * sampleRate;
        decaySamples   = decayTime * sampleRate;
        releaseSamples = releaseTime * sampleRate;
    }

    void noteOn(const NoteOnData& event) override {
        state = State::Attack;
        envelopeTime = 0.0f;
    }

    void noteOff() override {
        state = State::Release;
        envelopeTime = 0.0f;
        releaseStartLevel = currentLevel;
    }

    void reset() override {
        state = State::Idle;
        currentLevel = 0.0f;
        envelopeTime = 0.0f;
    }

    void applyMidi(const MidiMessage& message) override {
        // No MIDI CC handling for now
    }

    void processSample(float* sample) override {
        switch (state) {
            case State::Attack:
                if (envelopeTime < attackSamples) {
                    currentLevel = envelopeTime / attackSamples;
                } else {
                    currentLevel = 1.0f;
                    state = State::Decay;
                    envelopeTime = 0.0f;
                }
                break;

            case State::Decay:
                if (envelopeTime < decaySamples) {
                    float decayProgress = envelopeTime / decaySamples;
                    currentLevel = 1.0f - decayProgress * (1.0f - sustainLevel);
                } else {
                    currentLevel = sustainLevel;
                    state = State::Sustain;
                }
                break;

            case State::Sustain:
                currentLevel = sustainLevel;
                break;

            case State::Release:
                if (envelopeTime < releaseSamples) {
                    float releaseProgress = envelopeTime / releaseSamples;
                    currentLevel = releaseStartLevel * (1.0f - releaseProgress);
                } else {
                    currentLevel = 0.0f;
                    state = State::Idle;
                }
                break;

            case State::Idle:
                currentLevel = 0.0f;
                break;
        }

        envelopeTime += 1.0f;
        *sample *= currentLevel;
        internalBuffer.appendSingleSample(*sample);
    }

    String getName() override
    {
        return instanceId > 0 ? String("ASDR_") + String(instanceId) : "ASDR";
    }
    
    bool isActive() const
    {
        return state != State::Idle;
    }

private:
    enum class State {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    float sampleRate;

    float attackTime  = 0.01f;
    float decayTime   = 0.1f;
    float sustainLevel = 0.8f;
    float releaseTime = 0.5f;

    float attackSamples  = 441.0f;
    float decaySamples   = 4410.0f;
    float releaseSamples = 22050.0f;

    float envelopeTime;
    float currentLevel;
    float releaseStartLevel;
    
    int instanceId;

    State state;
};
*/
