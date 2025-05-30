# Available KPM Packages

KPM provides access to a variety of useful packages for your Kindle. This page lists the currently available packages that can be installed with KPM.

## How to Install Packages

To install any of these packages, use the following command on your Kindle:

```bash
kpm -S packagename
```

For example, to install KindleFetch:

```bash
kpm -S kindlefetch
```

## Available Packages

### blockamazon
A utility to block Amazon services on your Kindle, providing more privacy.

**Installation:**
```bash
kpm -S blockamazon
```

### gnomegames
A collection of classic GNOME games ported to Kindle.

**Installation:**
```bash
kpm -S gnomegames
```

### hello
A simple "Hello World" example package that demonstrates KPM's functionality.

**Installation:**
```bash
kpm -S hello
```

### hyprpad
A simple text editor for Kindle devices.

**Installation:**
```bash
kpm -S hyprpad
```

### kanki
A flashcard application for learning Japanese and other languages.

**Installation:**
```bash
kpm -S kanki
```

### kindlefetch
A system information tool similar to neofetch, showing information about your Kindle device.

**Installation:**
```bash
kpm -S kindlefetch
```

### kwordle
A Wordle game clone for Kindle.

**Installation:**
```bash
kpm -S kwordle
```

## Managing Installed Packages

To list all installed packages:
```bash
kpm -Ql
```

To check if a specific package is installed:
```bash
kpm -Ql packagename
```

To remove a package:
```bash
kpm -R packagename
```

To check if a package is available in configured repositories:
```bash
kpm -Qr packagename
```

To list all available packages in configured repositories:
```bash
kpm -Qr
```
