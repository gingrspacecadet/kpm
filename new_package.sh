#!/usr/bin/env bash
set -euo pipefail

read -rp "Enter package name (e.g. hello): " pkgname

if [[ -f "packages/$pkgname" ]]; then
  echo "ERROR: A file named \"$pkgname\" already exists in the packages directory!"
  exit 1
elif [[ -d "packages/$pkgname" ]]; then
  echo "ERROR: A directory named \"$pkgname\" already exists in the packages directory!"
  exit 1
elif [[ -z "$pkgname" ]]; then
  echo "ERROR: Package name cannot be empty!"
  exit 1
fi

mkdir -p "packages/$pkgname"

cat > packages/$pkgname/install.sh << EOF
#!/bin/sh
echo "Installing $pkgname..."
TEMP_DIR=\$(mktemp -d)
echo "Downloading $pkgname package..."
curl -fsSL https://github.com/you/package.zip -o "\$TEMP_DIR/$pkgname.zip"

echo "Extracting package..."
unzip -q "\$TEMP_DIR/$pkgname.zip" -d "\$TEMP_DIR"

echo "Installing to device..."
\"Your installation command here\"

rm -rf "\$TEMP_DIR"

echo "$pkgname package installed successfully."
EOF

cat > packages/$pkgname/install.sh << EOF
#!/bin/sh
echo "Uninstalling $pkgname..."
\"Your uninstallation command here\"
echo "$pkgname package uninstalled successfully."
EOF