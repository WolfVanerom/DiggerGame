# Prog4Game Engine - Wolf Vanerom

This is a rendition of the original Minigin project.  
It has been extended into a custom game engine used to build a Digger-style arcade game.

[![Build Status](https://github.com/WolfVanerom/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/WolfVanerom/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)

---

# Goal

The goal of this project is to build a functional and extensible game engine capable of running the game Digger.

---

# Engine Architecture

This engine is built around a modular architecture.

## Some Design Choices

### Command Pattern (Input System)
Input is fully decoupled from gameplay logic using the Command pattern.

- Keyboard and gamepad inputs are mapped to commands
- Supports single player, co-op, and versus modes
- Menu navigation is also handled through commands
- Every new category of comands is handled in a diffent list

---

### Observer Pattern
Was used for:

- Player → Score system
- Player → Life system

---

### State
Statemachines where used for:

- Gold bag
- Enemies

Largley because they cahange the most during gameplay.

---

### Multithreading 

Was used for the audio system to play without stopping gameplay.

---

### Service Locator
Global systems are accessed via a service locator to easily access seperate parts of the program:

- SoundSystem
- GameDataManager
- EnemySpawnManager
- PlayerAccessor

---

# Game Features

The engine supports the following gameplay features:

- Single player mode
- Co-op mode
- Versus mode
- Multi-level system (level loading from files)
- Enemy system (Nobbin / Hobbin behavior)
- Collectibles (emeralds, gold bags, bonus items)
- Scoring system
- Persistent high score system (file-based)
- Lives system
- Audio system (music + effects)
- Menu system with full input support

---

# Controls

## Menu Controls
- Keyboard: WASD + Arrow keys + Enter
- Controller: D-pad + A button

## Gameplay
- Player 1: WASD + Space (shoot)
- Player 2: Arrow keys + Right Shift (shoot)

Both players support keyboard and controller input.

---

# Build Instructions

## Windows
- Open the project in Visual Studio 2026 (CMake supported)
OR
- Use CMake + Ninja

## Web (Emscripten)
- Build using emcmake + emmake
- Run local Python server to test browser version

---

# Github Actions

This project uses GitHub Actions for continuous integration:

- CMake workflow: builds Windows Debug + Release
- Emscripten workflow: builds web version and publishes it as a GitHub Pages build

---

# Known Limitations

- Audio system is limited or disabled in web builds due to platform constraints
- Minor differences between desktop and web builds

---

# Repository

https://github.com/WolfVanerom/minigin
