#!/bin/bash
echo "Install hello"
mkdir -p /mnt/us/kpm/packages/hello
cp hello.sh /mnt/us/kpm/packages/hello/hello.sh
chmod +x /mnt/us/kpm/packages/hello/hello.sh
echo "Hello package installed successfully."