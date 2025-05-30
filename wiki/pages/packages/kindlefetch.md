# kindlefetch

## Overview
KindleFetch is a simple CLI tool that allows you to download books directly to your Kindle without needing a computer. It provides a convenient way to acquire reading material directly on your device.

## Prerequisites

**Your Kindle must be jailbroken before proceeding!**
- If your Kindle is not jailbroken, follow a jailbreaking guide first.

**Required components:**
- **KTerm**: Terminal application for Kindle
- **KUAL**: Kindle Unified Application Launcher
- **KOReader** (Optional but Recommended): Enhanced e-book reader

## Installation

### Using KPM (Recommended)
Install using KPM with:
```bash
kpm -S kindlefetch
```

### Manual Installation (Alternative)

#### 1. Install KTerm first (if not already installed):
- Download the latest release from KTerm's releases page
- Unzip the archive to the extensions directory in your Kindle's root

#### 2. Install KindleFetch directly:
Launch KTerm and run:
```bash
curl https://justrals.github.io/KindleFetch/install.sh | sh
```

Wait for the installation to complete. You should see a success message when finished.

## Usage
After installation, you can access KindleFetch through the KUAL menu. You should see a new "KindleFetch" option.

If requested by the application, update from the main menu.

## Uninstallation
To remove KindleFetch using KPM:
```bash
kpm -R kindlefetch
```

## Source
This package is based on the [KindleFetch](https://justrals.github.io/KindleFetch/) project.
