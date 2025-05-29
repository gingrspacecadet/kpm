#!/usr/bin/env bash
set -euo pipefail

read -rp "Enter package name (e.g. hello or * for all): " pkgname

# Get the list of package folders
if [[ "$pkgname" == "*" ]]; then
  pkgs=()
  while IFS= read -r -d '' dir; do
    pkgs+=("$(basename "$dir")")
  done < <(find packages/ -mindepth 1 -maxdepth 1 -type d -print0)
else
  pkgs=("$pkgname")
fi

for pkg in "${pkgs[@]}"; do
  pkgdir="packages/$pkg"
  zipfile="$pkg.zip"

  if [[ ! -d "$pkgdir" ]]; then
    echo "ERROR: Package folder \"$pkgdir\" not found!"
    continue
  fi

  if [[ -f "$zipfile" ]]; then
    rm "$zipfile"
  fi

  echo "Zipping contents of $pkgdir into $zipfile..."
  (cd "$pkgdir" && zip -r "../$zipfile" .)
  mv "packages/$zipfile" ./

  echo "Checking if tag/release \"$pkg\" exists..."

  # Delete local git tag if exists
  if git rev-parse "$pkg" >/dev/null 2>&1; then
    echo "Deleting local tag $pkg"
    git tag -d "$pkg"
  fi

  # Delete remote git tag if exists
  if git ls-remote --tags origin | grep -q "refs/tags/$pkg$"; then
    echo "Deleting remote tag $pkg"
    git push origin --delete "$pkg"
  fi

  # Delete existing GitHub release if exists
  if gh release view "$pkg" >/dev/null 2>&1; then
    echo "Deleting existing GitHub release $pkg"
    gh release delete "$pkg" -y
  fi

  echo "Creating tag and GitHub release \"$pkg\"..."

  # Create and push the new tag
  git tag "$pkg"
  git push origin "$pkg"

  # Create the GitHub release with the zip asset
  gh release create "$pkg" "$zipfile" --title "$pkg" --notes "Auto-uploaded package: $pkg" --latest

  echo "Package \"$pkg\" uploaded to GitHub release."

  rm "$zipfile"
done
