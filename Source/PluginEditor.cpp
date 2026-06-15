#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VolumeControllerAudioProcessorEditor::VolumeControllerAudioProcessorEditor (
        VolumeControllerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (320, 420);
    setResizable (false, false);

    // ── Volume Slider ─────────────────────────────────────────────────────
    volumeSlider.setSliderStyle (juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange (-60.0, 0.0, 0.1);
    volumeSlider.setValue (0.0);
    volumeSlider.setColour (juce::Slider::thumbColourId,      juce::Colour (0xff6c63ff));
    volumeSlider.setColour (juce::Slider::trackColourId,      juce::Colour (0xff6c63ff).withAlpha (0.6f));
    volumeSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (0xff2a2a3e));
    volumeSlider.onValueChange = [this] {
        dbReadout.setText (juce::String (volumeSlider.getValue(), 1) + " dB",
                           juce::dontSendNotification);
    };
    addAndMakeVisible (volumeSlider);

    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "volume", volumeSlider);

    // ── Labels ────────────────────────────────────────────────────────────
    volumeLabel.setText ("VOLUME", juce::dontSendNotification);
    volumeLabel.setFont (juce::Font (13.0f, juce::Font::bold));
    volumeLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
    volumeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (volumeLabel);

    dbReadout.setText ("0.0 dB", juce::dontSendNotification);
    dbReadout.setFont (juce::Font (16.0f, juce::Font::bold));
    dbReadout.setColour (juce::Label::textColourId, juce::Colour (0xff6c63ff));
    dbReadout.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (dbReadout);

    // ── Mute Button ───────────────────────────────────────────────────────
    muteButton.setClickingTogglesState (true);
    muteButton.setColour (juce::TextButton::buttonColourId,   juce::Colour (0xff2a2a3e));
    muteButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red.darker (0.2f));
    muteButton.setColour (juce::TextButton::textColourOffId,  juce::Colours::white);
    muteButton.setColour (juce::TextButton::textColourOnId,   juce::Colours::white);
    addAndMakeVisible (muteButton);

    muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "mute", muteButton);

    // ── Level Meter ───────────────────────────────────────────────────────
    addAndMakeVisible (levelMeter);

    // Repaint meter at ~30 Hz
    startTimerHz (30);
}

VolumeControllerAudioProcessorEditor::~VolumeControllerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void VolumeControllerAudioProcessorEditor::timerCallback()
{
    levelMeter.setLevel (audioProcessor.getCurrentVolume());

    // Keep dB readout in sync even when host automation moves the slider
    dbReadout.setText (juce::String (volumeSlider.getValue(), 1) + " dB",
                       juce::dontSendNotification);
}

//==============================================================================
void VolumeControllerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // ── Background gradient ───────────────────────────────────────────────
    juce::ColourGradient grad (juce::Colour (0xff0f0f1a), 0.0f, 0.0f,
                               juce::Colour (0xff1e1e3a), 0.0f, (float) getHeight(),
                               false);
    g.setGradientFill (grad);
    g.fillAll();

    // ── Title ─────────────────────────────────────────────────────────────
    g.setFont (juce::Font (20.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xff6c63ff));
    g.drawText ("Volume Controller", getLocalBounds().removeFromTop (50),
                juce::Justification::centred);

    // ── Subtle separator line ─────────────────────────────────────────────
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.fillRect (20, 52, getWidth() - 40, 1);
}

//==============================================================================
void VolumeControllerAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();

    // Title area: top 58 px
    // Meter strip: right column, 40 px wide
    // Slider: centre column
    // Label + readout: below slider
    // Mute button: bottom

    const int padding     = 20;
    const int meterW      = 28;
    const int sliderW     = 80;
    const int sliderH     = 220;
    const int topOffset   = 65;

    // Meter – right side
    levelMeter.setBounds (w - padding - meterW, topOffset, meterW, sliderH);

    // Slider – centred
    volumeSlider.setBounds ((w - sliderW) / 2, topOffset, sliderW, sliderH);

    // Label above slider
    volumeLabel.setBounds ((w - 120) / 2, topOffset - 24, 120, 20);

    // dB readout below slider
    dbReadout.setBounds ((w - 120) / 2, topOffset + sliderH + 8, 120, 26);

    // Mute button at the bottom
    muteButton.setBounds (padding, h - 55, w - padding * 2, 38);
}
