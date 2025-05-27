#!/bin/sh
set -e

echo "Installing kpm..."
wget https://raw.githubusercontent.com/gingrspacecadet/kpm/releases/kpm -O /usr/local/bin/kpm
chmod +x /usr/local/bin/kpm

echo "Creating config files..."
mkdir -p /etc/kpm
touch /etc/kpm/kpm_mirrors.conf
echo "https://raw.githubusercontent.com/gingrspacecadet/kpm/main/packages.conf https://github.com/gingrspacecadet/kpm/releases/download/{pkg}/{pkg}.zip" > /etc/kpm/kpm_mirrors.conf
touch /etc/kpm/kpm.conf
echo "INSTALL_DIR=/mnt/us/kpm/packages" >> /etc/kpm/kpm.conf
echo "MIRRORS_CONF=/etc/kpm/kpm_mirrors.conf" >> /etc/kpm/kpm.conf
echo "TMP_LIST_FILE=/tmp/kpm/kpm_packages.conf" >> /etc/kpm/kpm.conf

echo "Creating installation directories..."
mkdir -p /mnt/us/kpm
mkdir -p /mnt/us/kpm/packages
echo 'export PATH="/mnt/us/kpm/packages:$PATH"' >> ~/.bashrc