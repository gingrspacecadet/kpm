#!/usr/bin/env bash
set -euo pipefail

TAG="kpm"
FILE="out/kpm"
TITLE="kpm"
NOTES="Auto-uploaded kpm binary"

# Check if file exists
if [[ ! -f "$FILE" ]]; then
  echo "ERROR: File '$FILE' not found!"
  exit 1
fi

# Check if the tag exists locally or remotely
if git rev-parse "$TAG" >/dev/null 2>&1; then
  echo "Tag '$TAG' already exists locally."
else
  echo "Creating tag '$TAG'..."
  git tag "$TAG"
fi

if git ls-remote --tags origin | grep -q "refs/tags/$TAG$"; then
  echo "Tag '$TAG' already exists remotely."
else
  echo "Pushing tag '$TAG' to origin..."
  git push origin "$TAG"
fi

# Delete existing release if it exists
if gh release view "$TAG" >/dev/null 2>&1; then
  echo "Deleting existing GitHub release '$TAG'..."
  gh release delete "$TAG" -y
fi

# Create the new GitHub release with the file
echo "Creating GitHub release '$TAG' with asset '$FILE'..."
gh release create "$TAG" "$FILE" --title "$TITLE" --notes "$NOTES" --latest

echo "Release '$TAG' created successfully."
