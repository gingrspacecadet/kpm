#!/bin/sh
echo "Installing git..."
TEMP_DIR=$(mktemp -d)
echo "Downloading git package..."

echo "Extracting package..."

echo "Installing to device..."
cp /mnt/us/kpm/packages/git/git /usr/bin/git

rm -rf "$TEMP_DIR"

echo "git package installed successfully."