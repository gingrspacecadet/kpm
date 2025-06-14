#!/bin/sh
mv /mnt/us/kpm/packages/apk/apk /usr/bin/apk
mv /mnt/us/kpm/packages/apk/libcrypto.so.3 /usr/lib/libcrypto.so.3
mv /mnt/us/kpm/packages/apk/libapk.so.2.14.0 /lib/libapk.so.2.14.0
cp /mnt/us/kpm/packages/apk/libssl.so.3 /usr/lib/libssl.so.3
mv /mnt/us/kpm/packages/apk/libssl.so.3 /lib/libssl.so.3