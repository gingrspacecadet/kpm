#!/bin/sh
set -e

echo "Installing kpm..."
wget https://raw.githubusercontent.com/gingrspacecadet/kpm/releases/kpm -O /usr/local/bin/kpm
chmod +x /usr/local/bin/kpm

echo "Creating config files..."
touch /etc/kpm/kpm_mirrors.conf
touch /etc/kpm/kpm.conf

echo "Creating installation directories..."
mkdir -p /mnt/us/kpm
mkdir -p /mnt/us/kpm/packages
echo 'export PATH="/mnt/us/kpm/packages:$PATH"' >> ~/.bashrc