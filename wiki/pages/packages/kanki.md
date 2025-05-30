# kanki

## Overview
Kanki is a flashcard application designed for learning Japanese and other languages on your Kindle device.

## Features
- Flashcard system for language learning
- Optimized for e-ink displays
- Supports Japanese characters and other languages
- Customizable card sets
- Spaced repetition system

## Installation
Install using KPM with:
```bash
kpm -S kanki
```

## Usage
After installation, you'll find `kanki.sh` in your documents folder. Tap on it to launch the application.

### Basic Controls
- Tap the right side of the screen to reveal the answer/next card
- Tap the left side to go back
- Use the menu button for additional options

## Creating Your Own Card Sets
You can create custom card sets by placing text files in the `/mnt/us/documents/kanki/cards/` directory. The format is:
```
Front side of card|Back side of card
```

For example:
```
犬|dog
猫|cat
```

## Uninstallation
To remove kanki, use:
```bash
kpm -R kanki
```

## Source
This package is based on the [KAnki](https://github.com/crizmo/KAnki) project by crizmo.
