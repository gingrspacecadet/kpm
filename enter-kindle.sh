#!/usr/bin/env bash
ROOTFS="alpine-rootfs"

echo "Entering ARMv7 Alpine shell…"
exec proot                                                  \
     -r "$ROOTFS"                                           \
     -q /usr/bin/qemu-arm                                   \
     -b /dev -b /proc -b /sys                               \
     env PATH=/sbin:/usr/sbin:/usr/bin:/bin:/usr/local/bin /bin/sh         \
