#!/bin/sh
mkdir -p /mnt/us/extensions/kpm
START=/mnt/us/kpm/packages/kpmgui/
END=/mnt/us/extensions/kpm
cp $START/config.xml $END
cp $START/menu.json $END
cp $START/bin/kpm_ui $END/bin/kpm_ui
