#!/bin/sh
set -euo pipefail
mntroot rw

# Remove old files
if [ -f /usr/local/bin/kpm ]; then
  rm /usr/local/bin/kpm
fi

# Installation directories for compat
mkdir -p /usr/local
mkdir -p /usr/local/bin

# Download the kpm binary
echo "Installing kpm to /usr/local/bin..."
curl -L https://github.com/gingrspacecadet/kpm/releases/download/kpm/kpm -o /usr/local/bin/kpm
chmod +x /usr/local/bin/kpm

# Installed packages list
mkdir -p /mnt/us/kpm
touch /mnt/us/kpm/packages.list

# Config files
echo "Creating config files..."
mkdir -p /etc/kpm
cat > /etc/kpm/kpm_mirrors.conf << 'EOF'
https://raw.githubusercontent.com/gingrspacecadet/kpm/refs/heads/main/packages.list https://github.com/gingrspacecadet/kpm/releases/download/{pkg}/{pkg}.zip
EOF

cat > /etc/kpm/kpm.conf << 'EOF'
# Example config file
# Installation directory
INSTALL_DIR=/mnt/us/kpm/packages
# Mirror config file
MIRRORS_CONF=/etc/kpm/kpm_mirrors.conf
# Package cache directory
TMP_LIST_FILE=/tmp/kpm/kpm_packages.list
EOF

# Download directories
echo "Creating download & install directories..."
mkdir -p /mnt/us/kpm
mkdir -p /mnt/us/kpm/packages
mkdir -p /mnt/us/kpm/packages/bin
mkdir -p /tmp/kpm

echo "Configuring PATH in ~/.bashrc..."

# Ensure ~/.bashrc exists
if [ ! -f ~/.bashrc ]; then
  touch ~/.bashrc
fi

# Add export only if not already present
grep -qxF 'export PATH="/mnt/us/kpm/packages/bin:$PATH"' /etc/profile || \
  echo 'export PATH="/mnt/us/kpm/packages/bin:$PATH"' >> /etc/profile

source ~/.bashrc

kpm -S kpmgui

echo "All done! You can now run 'kpm'."
