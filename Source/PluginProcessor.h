#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class VolumeControllerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==========================================================================
    VolumeControllerAudioProcessor();
    ~VolumeControllerAudioProcessor() override;

    //==========================================================================
    void prepareToPlay  (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==========================================================================
    const juce::String getName() const override { return JucePlugin_Name; }

    bool   acceptsMidi()  const override { return false; }
    bool   producesMidi() const override { return false; }
    bool   isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==========================================================================
    int  getNumPrograms()    override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==========================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==========================================================================
    // Parameter tree – public so the editor can attach to it
    juce::AudioProcessorValueTreeState apvts;

    // Convenience accessors (read from APVTS each block)
    float getCurrentVolume() const { return currentVolume.load(); }
    bool  getIsMuted()       const { return isMuted.load(); }

private:
    //==========================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Smoothed gain so there are no clicks on parameter changes
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;

    // Cached values (written in processBlock, read in editor for metering)
    std::atomic<float> currentVolume { 0.0f };
    std::atomic<bool>  isMuted       { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeControllerAudioProcessor)
};
