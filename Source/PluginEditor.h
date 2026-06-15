#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginProcessor.h"

//==============================================================================
/** A simple vertical level-meter component. */
class LevelMeter  : public juce::Component
{
public:
    void setLevel (float rmsLinear)
    {
        // Smooth the display a little
        displayLevel = displayLevel * 0.85f + rmsLinear * 0.15f;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour (juce::Colour (0xff1a1a2e));
        g.fillRoundedRectangle (bounds, 4.0f);

        // Convert linear → dB for display
        const float dB        = juce::Decibels::gainToDecibels (displayLevel, -60.0f);
        const float normLevel = juce::jmap (dB, -60.0f, 0.0f, 0.0f, 1.0f);
        const float barHeight = bounds.getHeight() * normLevel;

        // Colour: green → yellow → red
        juce::Colour barColour = (dB > -6.0f)  ? juce::Colours::red
                               : (dB > -18.0f) ? juce::Colours::yellow
                                               : juce::Colour (0xff00e676);

        juce::Rectangle<float> bar (bounds.getX(),
                                    bounds.getBottom() - barHeight,
                                    bounds.getWidth(),
                                    barHeight);
        g.setColour (barColour);
        g.fillRoundedRectangle (bar, 3.0f);

        // Outline
        g.setColour (juce::Colours::white.withAlpha (0.15f));
        g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
    }

private:
    float displayLevel = 0.0f;
};

//==============================================================================
class VolumeControllerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit VolumeControllerAudioProcessorEditor (VolumeControllerAudioProcessor&);
    ~VolumeControllerAudioProcessorEditor() override;

    void paint  (juce::Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    void timerCallback() override;

    //==========================================================================
    VolumeControllerAudioProcessor& audioProcessor;

    // ── Widgets ──────────────────────────────────────────────────────────────
    juce::Slider volumeSlider;
    juce::Label  volumeLabel;
    juce::Label  dbReadout;

    juce::TextButton muteButton { "MUTE" };

    LevelMeter levelMeter;

    // ── APVTS Attachments ────────────────────────────────────────────────────
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeControllerAudioProcessorEditor)
};
