#!/usr/bin/env bash
set -euo pipefail

echo "Installing kpm to /usr/local/bin..."
# Download the kpm binary
wget -q https://github.com/gingrspacecadet/kpm/releases/download/kpm/kpm -O /usr/local/bin/kpm
chmod +x /usr/local/bin/kpm

echo "Creating /etc/kpm and config files..."
mkdir -p /etc/kpm
cat > /etc/kpm/kpm_mirrors.conf << 'EOF'
https://raw.githubusercontent.com/gingrspacecadet/kpm/main/packages.conf https://github.com/gingrspacecadet/kpm/releases/download/{pkg}/{pkg}.zip
EOF

cat > /etc/kpm/kpm.conf << 'EOF'
INSTALL_DIR=/mnt/us/kpm/packages
MIRRORS_CONF=/etc/kpm/kpm_mirrors.conf
TMP_LIST_FILE=/tmp/kpm/kpm_packages.conf
EOF

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
grep -qxF 'export PATH="/mnt/us/kpm/packages/bin:$PATH"' ~/.bashrc || \
  echo 'export PATH="/mnt/us/kpm/packages/bin:$PATH"' >> ~/.bashrc

source ~/.bashrc
echo "All done! You can now run 'kpm'."
