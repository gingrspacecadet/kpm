#!/usr/bin/env bash
set -e

# 1) Ensure we have qemu-user emulators
sudo pacman -Sy --noconfirm --needed qemu-user wget

# 2) Config
ARCH="armhf"
ALPINE_VERSION="3.18.0"
ROOTFS_URL="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION%.*}/releases/${ARCH}/alpine-minirootfs-${ALPINE_VERSION}-${ARCH}.tar.gz"
TARBALL="alpine-minirootfs-${ALPINE_VERSION}-${ARCH}.tar.gz"
ROOTFS_DIR="alpine-rootfs"

# 3) Download minirootfs if needed
if [ ! -f "$TARBALL" ]; then
  echo "[*] Downloading Alpine ARMv7 minirootfs..."
  wget -q "$ROOTFS_URL" -O "$TARBALL"
fi

# 4) Unpack into alpine-rootfs/
echo "[*] Unpacking rootfs to $ROOTFS_DIR/"
rm -rf "$ROOTFS_DIR"
mkdir -p "$ROOTFS_DIR"
tar -xzf "$TARBALL" -C "$ROOTFS_DIR"

# 5) Set up resolv.conf so networking works inside
echo "nameserver 1.1.1.1" > "$ROOTFS_DIR/etc/resolv.conf"

# 6) Create launcher script
cat > enter-kindle.sh << 'EOF'
#!/usr/bin/env bash
# Drop into ARMv7 Alpine shell using host qemu-arm
echo "Entering ARMv7 Alpine shell (Kindle‐like)..."
exec qemu-arm -L alpine-rootfs /bin/sh
EOF
chmod +x enter-kindle.sh

echo
echo "✅ Setup complete!"
echo "→ To enter your ARMv7 environment, run:"
echo "    ./enter-kindle.sh"
echo
echo "Inside that shell you can test any ARMv7 binary by simply running it."
