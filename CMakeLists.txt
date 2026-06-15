cmake_minimum_required(VERSION 3.22)
project(VolumeControllerPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE ──────────────────────────────────────────────────────────────────────
add_subdirectory(JUCE)

juce_add_plugin(VolumeControllerPlugin
    COMPANY_NAME                "MyCompany"
    PLUGIN_MANUFACTURER_CODE    MyCo
    PLUGIN_CODE                 Vlm1
    FORMATS                     VST3 Standalone
    PRODUCT_NAME                "Volume Controller"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE
)

# ── Generate JuceHeader.h ──────────────────────────────────────────────────────
juce_generate_juce_header(VolumeControllerPlugin)

# ── Source files ───────────────────────────────────────────────────────────────
target_sources(VolumeControllerPlugin
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# ── Compile definitions ────────────────────────────────────────────────────────
target_compile_definitions(VolumeControllerPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ── Link libraries ─────────────────────────────────────────────────────────────
target_link_libraries(VolumeControllerPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
