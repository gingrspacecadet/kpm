#!/bin/sh
echo "Installing kwordle..."
TEMP_DIR=$(mktemp -d)
echo "Downloading kwordle package..."
curl -L https://github.com/crizmo/KWordle/releases/download/v1.2.0/kwordle.zip -o "$TEMP_DIR/kwordle.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/kwordle.zip" -d "$TEMP_DIR"

echo "Installing to device..."
mkdir -p /mnt/us/documents/kwordle
cp -r "$TEMP_DIR/kwordle"/* /mnt/us/documents/kwordle/
chmod +x /mnt/us/documents/kwordle/kwordle.sh

rm -rf "$TEMP_DIR"

echo "kwordle package installed successfully."