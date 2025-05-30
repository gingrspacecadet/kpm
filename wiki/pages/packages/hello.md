# hello

## Overview
The "hello" package is a simple example package that demonstrates KPM's package management capabilities. It's useful for testing whether KPM is working correctly.

## Features
- Minimal "Hello World" program
- Demonstrates KPM installation functionality
- Useful for testing and learning about KPM

## Installation
Install using KPM with:
```bash
kpm -S hello
```

## Usage
After installation, you can run the hello program from the terminal:
```bash
hello
```

The program will output:
```
Hello, World!
```

## Uninstallation
To remove the hello package, use:
```bash
kpm -R hello
```

## Creating Your Own Hello Package
This package serves as a simple template for creating your own KPM packages. To create a similar package:

1. Create a directory with your package name
2. Add an executable file
3. Create install.sh and uninstall.sh scripts
4. Package it with KPM's upload_package.sh script

For more details, see the [Creating Packages](/tutorials/creating-packages) tutorial.
