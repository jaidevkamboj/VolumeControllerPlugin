#!/usr/bin/env bash
# build.sh – one-shot build helper for VolumeControllerPlugin
set -e

echo "=== Volume Controller Plugin – Build Script ==="

# 1. Clone JUCE if not already present
if [ ! -d "JUCE" ]; then
    echo "[1/3] Cloning JUCE..."
    git clone --depth 1 https://github.com/juce-framework/JUCE.git JUCE
else
    echo "[1/3] JUCE already present – skipping clone."
fi

# 2. Configure
echo "[2/3] Configuring with CMake..."
cmake -B build \
      -DCMAKE_BUILD_TYPE=Release \
      -G "Unix Makefiles"   # change to "Xcode" on macOS or "Visual Studio 17 2022" on Windows

# 3. Build
echo "[3/3] Building..."
cmake --build build --config Release --parallel

echo ""
echo "✅  Build complete!"
echo "   VST3  : build/VolumeControllerPlugin_artefacts/Release/VST3/"
echo "   App   : build/VolumeControllerPlugin_artefacts/Release/Standalone/"
