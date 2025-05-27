set -e

echo "Installing kpm..."
wget https://raw.githubusercontent.com/gingrspacecadet/kpm/releases/kpm -O /usr/local/bin/kpm
chmod +x /usr/local/bin/kpm

echo "Creating config files..."
touch /etc/kpm_mirrors.conf

echo "Creating installation directories..."
mkdir -p /mnt/us/kpm
mkdir -p /mnt/us/kpm/packages
echo 'export PATH="/mnt/us/kpm/packages:$PATH"' >> ~/.bashrc