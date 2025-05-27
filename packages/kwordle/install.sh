#!/bin/sh
set -eu

# ---- Ensure Unix line endings ----
# (If you ever edit this on Windows, run: dos2unix install.sh)

TMPDIR="/tmp/kpm"
DSTBASE="/mnt/us/documents"
PKG="kwordle"
PKGZIP="$TMPDIR/${PKG}.zip"
PKGDIR="$TMPDIR/$PKG"

echo "Installing $PKG..."

# ---- Prepare staging dir ----
echo "  * Creating staging directory $TMPDIR"
mkdir -p "$TMPDIR"

# ---- Download ----
echo "  * Downloading $PKG package to $PKGZIP"
wget -q https://github.com/crizmo/KWordle/releases/download/v1.2.0/kwordle.zip \
     -O "$PKGZIP"

# ---- Extract ----
echo "  * Extracting package..."
unzip -o "$PKGZIP" -d "$TMPDIR"

# ---- Install to device ----
echo "  * Installing to device..."
mkdir -p "$DSTBASE/$PKG"
cp -r "$PKGDIR/." "$DSTBASE/$PKG/"
cp "$TMPDIR/${PKG}.sh" "$DSTBASE/"

echo "  * Making launcher executable"
chmod +x "$DSTBASE/$PKG/$PKG.sh"

echo "Done! $PKG is installed under $DSTBASE/$PKG"
