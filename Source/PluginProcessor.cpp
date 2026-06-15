#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
    VolumeControllerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Volume: 0 dB .. -60 dB (default 0 dB)
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "volume",                          // paramID
        "Volume",                          // display name
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f),
        0.0f,                              // default
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) {
                return juce::String (v, 1) + " dB";
            })
    ));

    // Mute toggle (default: unmuted)
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "mute", "Mute", false
    ));

    return layout;
}

//==============================================================================
VolumeControllerAudioProcessor::VolumeControllerAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

VolumeControllerAudioProcessor::~VolumeControllerAudioProcessor() {}

//==============================================================================
void VolumeControllerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Smooth over ~20 ms to avoid zipper noise
    smoothedGain.reset (sampleRate, 0.02);
    smoothedGain.setCurrentAndTargetValue (1.0f);

    ignoreUnused (samplesPerBlock);
}

void VolumeControllerAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VolumeControllerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Mono or stereo only; in == out
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}
#endif

//==============================================================================
void VolumeControllerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                   juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    ignoreUnused (midiMessages);

    // --- Read current parameter values ---
    const float volumeDb = apvts.getRawParameterValue ("volume")->load();
    const bool  muted    = apvts.getRawParameterValue ("mute")->load() > 0.5f;

    isMuted.store (muted);

    // Convert dB → linear and apply mute
    const float targetGain = muted ? 0.0f : juce::Decibels::decibelsToGain (volumeDb);
    smoothedGain.setTargetValue (targetGain);

    // --- Apply sample-accurate smoothed gain ---
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* data = buffer.getWritePointer (ch);

        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            // Only advance the smoother once per sample (use channel 0 to drive it)
            const float gain = (ch == 0) ? smoothedGain.getNextValue()
                                         : smoothedGain.getCurrentValue();
            data[s] *= gain;
        }

        // Reset smoother position for channel 1+ so they read the same ramp
        if (ch == 0)
            smoothedGain.setCurrentAndTargetValue (smoothedGain.getCurrentValue());
    }

    // Store post-gain RMS for the level meter in the editor
    currentVolume.store (buffer.getRMSLevel (0, 0, buffer.getNumSamples()));
}

//==============================================================================
juce::AudioProcessorEditor* VolumeControllerAudioProcessor::createEditor()
{
    return new VolumeControllerAudioProcessorEditor (*this);
}

//==============================================================================
void VolumeControllerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VolumeControllerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// Plugin entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VolumeControllerAudioProcessor();
}
