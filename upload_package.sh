#!/usr/bin/env bash
set -euo pipefail

read -rp "Enter package name (e.g. hello): " pkgname

pkgdir="packages/$pkgname"
zipfile="$pkgname.zip"

if [[ ! -d "$pkgdir" ]]; then
  echo "ERROR: Package folder \"$pkgdir\" not found!"
  exit 1
fi

if [[ -f "$zipfile" ]]; then
  rm "$zipfile"
fi

echo "Zipping $pkgdir into $zipfile..."
zip -r "$zipfile" "$pkgdir"
if [[ $? -ne 0 ]]; then
  echo "ERROR: Failed to zip package"
  exit 1
fi

echo "Checking if tag/release \"$pkgname\" exists..."

# Delete local git tag if exists
if git rev-parse "$pkgname" >/dev/null 2>&1; then
  echo "Deleting local tag $pkgname"
  git tag -d "$pkgname"
fi

# Delete remote git tag if exists
if git ls-remote --tags origin | grep -q "refs/tags/$pkgname$"; then
  echo "Deleting remote tag $pkgname"
  git push origin --delete "$pkgname"
fi

# Delete existing GitHub release if exists (ignore error if not found)
if gh release view "$pkgname" >/dev/null 2>&1; then
  echo "Deleting existing GitHub release $pkgname"
  gh release delete "$pkgname" -y
fi

echo "Creating tag and GitHub release \"$pkgname\"..."

# Create and push the new tag
git tag "$pkgname"
git push origin "$pkgname"

# Create the GitHub release with the zip asset
gh release create "$pkgname" "$zipfile" --title "$pkgname" --notes "Auto-uploaded package: $pkgname" --latest

echo "Package \"$pkgname\" uploaded to GitHub release."

rm "$zipfile"
