#!/bin/sh
echo "Installing gambatte..."
TEMP_DIR=$(mktemp -d)
echo "Downloading gambatte package..."
curl -fsSL https://github.com/crazy-electron/gambatte-k2/releases/latest/download/gambatte-k2.zip -o "$TEMP_DIR/gambatte.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/gambatte.zip" -d "$TEMP_DIR"

echo "Installing to device..."
mkdir -p /mnt/us/extensions
cp -r "$TEMP_DIR"/* /mnt/us/extensions/

rm -rf "$TEMP_DIR"

echo "gambatte package installed successfully."