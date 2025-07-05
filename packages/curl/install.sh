#!/bin/sh
echo "Installing curl..."

cp /mnt/us/kpm/packages/curl/curl /usr/bin/curl
cp /mnt/us/kpm/packages/curl/libcurl.so.4 /usr/lib/

echo "curl package installed successfully."