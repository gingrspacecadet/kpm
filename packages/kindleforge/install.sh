#!/bin/sh
echo "Installing kindleforge..."
TEMP_DIR=/tmp/kpm
echo "Downloading kindleforge package..."
curl -L https://github.com/polish-penguin-dev/KindleForge/archive/refs/heads/master.zip -o "$TEMP_DIR/kindleforge.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/kindleforge.zip" -d "$TEMP_DIR"

echo "Installing to device..."
mkdir -p /mnt/us/documents/kindleforge
cp -r "$TEMP_DIR/KindleForge-master"/* /mnt/us/documents/kindleforge/
cp "$TEMP_DIR"/KindleForge.sh /mnt/us/documents/
chmod +x /mnt/us/documents/KindleForge.sh

rm -rf "$TEMP_DIR"

echo "kindleforge package installed successfully."