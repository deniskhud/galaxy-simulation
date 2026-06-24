# Galactic — GPU-driven Galaxy Particle Simulation
 ![C++](https://img.shields.io/badge/C%2B%2B-23-blue?style=flat-square&logo=cplusplus)
![Vulkan](https://img.shields.io/badge/Vulkan-1.3-red?style=flat-square&logo=vulkan)
![SDL3](https://img.shields.io/badge/SDL-3-green?style=flat-square)
![Slang](https://img.shields.io/badge/Shaders-Slang-orange?style=flat-square)
![ImGui](https://img.shields.io/badge/UI-ImGui-yellow)

A real-time galaxy simulation engine built with Vulkan, featuring millions of particles driven entirely by compute shaders.

<video src="https://github.com/user-attachments/assets/ef93a809-f790-4dc5-a41e-c4a664995266" controls width="100%"></video>
![screenshot](https://github.com/user-attachments/assets/37305025-dbfc-438e-b4e1-91e712240f76)

# Features
 
- **Vulkan 1.3 vk::raii wrappers** 
- full calculation of particles on the **gpu** side
- **ImGui** - changing galaxy settings in real time

## How it works
 
Two compute shaders run back-to-back every frame:
 
```
[computeInit.comp]  — runs once on startup or when parameters change
       │   BufferMemoryBarrier
       ▼
[compute.comp]      — runs every frame, integrates elliptical orbits
       │   BufferMemoryBarrier
       ▼
[vert + frag]       — reads particle SSBO, renders soft glow point sprites
```
 
Orbital velocity follows a flat rotation curve approximation matching observed galaxy kinematics:
 
```
ω(r) = v_max / √(r² + r_core²)
```
 
Star colors are computed from blackbody temperature (3000–9000 K) using the Tanner approximation.
 
---





## Build

### Dependencies
 
- Vulkan SDK 1.3+
- SDL3
- Dear ImGui
- GLM
- [Slang](https://github.com/shader-slang/slang)
- CMake 3.25+


### Setup

1. clone this repo: 
```
git clone https://github.com/deniskhud/Vulkan-engine.git
```

2. create build folder:
```
mkdir build && cd build
```

3. CMake 
```
cmake .. && make -j$(nproc)
```

4. run
```
./galactic
```

### Shaders
 
Shaders are written in [Slang](https://github.com/shader-slang/slang) and compiled to SPIR-V. CMake compiles them automatically as part of the build via `add_custom_command`. To recompile manually:
 
```bash
slangc src/shaders/computeInit.slang -o src/shaders/computeInit.spv -target spirv
slangc src/shaders/compute.slang     -o src/shaders/comp.spv         -target spirv
slangc src/shaders/galaxy.slang      -entry VSMain -o src/shaders/vert.spv -target spirv
slangc src/shaders/galaxy.slang      -entry PSMain -o src/shaders/frag.spv -target spirv
```

## Roadmap
 
-  Nebula / dust cloud rendering (volumetric)
-  Multiple galaxy interaction (collision simulation)
-  Dark matter halo influence on rotation curve
-  LOD system for extreme particle counts
-  Save / load galaxy presets
---







> The project was inspired by this video https://youtu.be/Pj1P0zV4zDI
