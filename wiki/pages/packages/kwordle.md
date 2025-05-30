# kwordle

## Overview
Kwordle is a Wordle game clone developed specifically for Kindle devices, allowing you to enjoy the popular word-guessing game on your e-reader.

## Features
- Classic Wordle gameplay
- Optimized for e-ink displays
- Multiple difficulty levels
- Keeps track of your statistics
- Works offline

## Installation
Install using KPM with:
```bash
kpm -S kwordle
```

## How to Play
1. After installation, you'll find `kwordle.sh` in your documents folder.
2. Tap on it to launch the game.
3. The objective is to guess a 5-letter word within 6 attempts.
4. After each guess, you'll see:
   - Letters in the correct position marked green
   - Letters in the word but in the wrong position marked yellow
   - Letters not in the word marked gray

## Uninstallation
To remove kwordle, use:
```bash
kpm -R kwordle
```

## Source
This package is based on the [kwordle](https://github.com/crizmo/kwordle) project by crizmo.
