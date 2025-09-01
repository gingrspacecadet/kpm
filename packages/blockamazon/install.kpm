#!/bin/sh
echo "Installing blockamazon..."
TEMP_DIR=$(mktemp -d)
echo "Downloading blockamazon package..."
curl -L https://github.com/mitchellurgero/kindle-kual-blockamazon/archive/refs/tags/1.0.0.zip -o "$TEMP_DIR/blockamazon.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/blockamazon.zip" -d "$TEMP_DIR"

echo "Installing to device..."
mkdir -p /mnt/us/documents/blockamazon
cp -r "$TEMP_DIR/blockamazon/blockamazon"/* /mnt/us/extensions

rm -rf "$TEMP_DIR"

echo "blockamazon package installed successfully."