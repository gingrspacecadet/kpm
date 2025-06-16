#!/bin/sh
echo "Installing kanki..."
TEMP_DIR=$(mktemp -d)
echo "Downloading kanki package..."
curl -L https://github.com/crizmo/KAnki/releases/download/v1.1.0/kanki.zip -o "$TEMP_DIR/kanki.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/kanki.zip" -d "$TEMP_DIR"

echo "Installing to device..."
mkdir -p /mnt/us/documents/kanki
cp -r "$TEMP_DIR/kanki"/* /mnt/us/documents/kanki/
cp "$TEMP_DIR"/kanki.sh /mnt/us/documents/
chmod +x /mnt/us/documents/kanki.sh

rm -rf "$TEMP_DIR"

echo "kanki package installed successfully."