#!/bin/sh
echo "Installing git..."
TEMP_DIR=$(mktemp -d)
echo "Downloading git package..."

echo "Extracting package..."

echo "Installing to device..."
cp /mnt/us/kpm/packages/git/git /usr/bin/git
cp -r /mnt/us/kpm/packages/git/git-core /usr/libexec/

rm -rf "$TEMP_DIR"

echo "git package installed successfully."