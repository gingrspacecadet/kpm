#!/bin/sh
echo "Installing gnome games..."
TEMP_DIR=$(mktemp -d)
echo "Downloading gnomegames package..."
curl -L https://github.com/crazy-electron/GnomeGames4Kindle/releases/latest/download/gnomegames.zip -o "$TEMP_DIR/gnomegames.zip"

echo "Extracting package..."
unzip -q "$TEMP_DIR/gnomegames.zip" -d "$TEMP_DIR"
mkdir -p /mnt/us/extensions/gnomegames
cp -r "$TEMP_DIR/gnomegames"/* /mnt/us/extensions/gnomegames

echo "Installing to device..."
cp "/mnt/us/extensions/gnomegames/shortcut_gnomechess.sh" "/mnt/us/documents"
cp "/mnt/us/extensions/gnomegames/shortcut_gnomine.sh" "/mnt/us/documents"

rm -rf "$TEMP_DIR"

echo "gnomegames package installed successfully."