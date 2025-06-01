#!/bin/sh
START=/mnt/us/kpm/packages/kpmgui/
END=/mnt/us/extensions/kpm
mkdir -p /mnt/us/extensions/kpm
mkdir -p $END/bin
cp $START/config.xml $END
cp $START/menu.json $END
