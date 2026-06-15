# Volume Controller Plugin

A clean, fully working **JUCE VST3 / Standalone** audio plugin with:

| Feature | Detail |
|---|---|
| Volume fader | −60 dB → 0 dB, linear-vertical slider |
| Mute button | Toggle; turns red when active |
| Level meter | RMS, colour-coded (green / yellow / red) |
| Smooth gain | 20 ms ramp – zero zipper noise |
| State save/load | Full APVTS XML persistence |
| Formats | VST3 + Standalone |

---

## Prerequisites

| Tool | Version |
|---|---|
| CMake | ≥ 3.22 |
| C++ compiler | MSVC 2022 / Clang 14+ / GCC 11+ |
| Git | any |

---

## Quick Start

```bash
# 1. Clone or unzip the project
cd VolumeControllerPlugin

# 2. Clone JUCE (first time only)
git clone --depth 1 https://github.com/juce-framework/JUCE.git JUCE

# 3a. Linux / WSL
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel

# 3b. macOS (Xcode)
cmake -B build -G Xcode
cmake --build build --config Release

# 3c. Windows (Visual Studio 2022)
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Or just run **`bash build.sh`** on Linux/macOS.

---

## Output locations (after build)

```
build/VolumeControllerPlugin_artefacts/Release/
├── VST3/   VolumeController.vst3   ← copy to your VST3 folder
└── Standalone/  VolumeController   ← run directly
```

### Default VST3 install paths
| OS | Path |
|---|---|
| Windows | `C:\Program Files\Common Files\VST3\` |
| macOS | `~/Library/Audio/Plug-Ins/VST3/` |
| Linux | `~/.vst3/` |

---

## Parameter reference

| ID | Range | Default | Unit |
|---|---|---|---|
| `volume` | −60 → 0 | 0 | dB |
| `mute` | false / true | false | – |

---

## Project structure

```
VolumeControllerPlugin/
├── CMakeLists.txt          ← build config
├── build.sh                ← quick-build helper
├── README.md
├── JUCE/                   ← git clone here
└── Source/
    ├── PluginProcessor.h   ← audio engine + APVTS
    ├── PluginProcessor.cpp
    ├── PluginEditor.h      ← GUI declarations
    └── PluginEditor.cpp    ← GUI implementation
```
