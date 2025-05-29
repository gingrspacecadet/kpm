#!/usr/bin/env bash
ROOTFS="alpine-rootfs"

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
