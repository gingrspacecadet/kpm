#!/bin/sh
echo "Installing kwordle..."
TEMP_DIR=/tmp/kpm
echo "Downloading kwordle package..."
wget https://github.com/crizmo/KWordle/releases/download/v1.2.0/kwordle.zip -O $TEMP_DIR/kwordle.zip

echo "Extracting package..."
unzip $TEMP_DIR/kwordle.zip -d $TEMP_DIR

echo "Installing to device..."
mkdir -p /mnt/us/documents/kwordle
cp -r $TEMP_DIR/kwordle/ /mnt/us/documents/kwordle/
cp $TEMP_DIR/kwordle.sh /mnt/us/documents/
chmod +x /mnt/us/documents/kwordle/kwordle.sh
