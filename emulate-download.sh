#!/usr/bin/env bash
set -e

# 2) Config
ARCH="armhf"
ALPINE_VERSION="3.18.0"
ROOTFS_URL="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION%.*}/releases/${ARCH}/alpine-minirootfs-${ALPINE_VERSION}-${ARCH}.tar.gz"
TARBALL="alpine-minirootfs-${ALPINE_VERSION}-${ARCH}.tar.gz"
ROOTFS_DIR="alpine-rootfs"
ROOTFS_PATH="$HOME/$ROOTFS_DIR"

# 3) Download minirootfs if needed
if [ ! -f "$TARBALL" ]; then
  echo "[*] Downloading Alpine ARMv7 minirootfs..."
  wget -q "$ROOTFS_URL" -O "$TARBALL"
fi

# 4) Unpack into $HOME/alpine-rootfs/
echo "[*] Unpacking rootfs to $ROOTFS_PATH/"
rm -rf "$ROOTFS_PATH"
mkdir -p "$ROOTFS_PATH"
tar -xzf "$TARBALL" -C "$ROOTFS_PATH"

# 5) Set up resolv.conf so networking works inside
echo "nameserver 1.1.1.1" > "$ROOTFS_PATH/etc/resolv.conf"

# 6) Create launcher script
cat > enter-kindle.sh << 'EOF'
#!/usr/bin/env bash
ROOTFS="$HOME/alpine-rootfs"

if [[ "$1" == "-c" && -n "$2" ]]; then
    CMD="$2"
    echo "Running command in ARMv7 Alpine environment: $CMD"
    exec proot                                                  \
         -r "$ROOTFS"                                           \
         -q /usr/bin/qemu-arm                                   \
         -b /dev -b /proc -b /sys                               \
         env PATH=/sbin:/usr/sbin:/usr/bin:/bin:/usr/local/bin /bin/sh -c "$CMD"
else
    echo "Entering ARMv7 Alpine shell…"
    exec proot                                                  \
         -r "$ROOTFS"                                           \
         -q /usr/bin/qemu-arm                                   \
         -b /dev -b /proc -b /sys                               \
         env PATH=/mnt/us/kpm/packages/bin:/sbin:/usr/sbin:/usr/bin:/bin:/usr/local/bin /bin/sh
fi
EOF
chmod +x enter-kindle.sh

rm $TARBALL

echo
echo "✅ Setup complete!"
echo "→ To enter your ARMv7 environment, run:"
echo "    ./enter-kindle.sh"
echo
echo "Inside that shell you can test any ARMv7 binary by simply running it."
