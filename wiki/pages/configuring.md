## Configuring KPM

This guide will walk you through the process of configuring KPM on your Kindle device.

### Config file
The config file is located at `/etc/kpm/kpm.conf`. It is structured as a key-value file, i.e. `VARIABLE="Hello"`. By default, the contents of this file is as follows:
```conf
# Example config file
# Installation directory
INSTALL_DIR=/mnt/us/kpm/packages
# Mirror config file
MIRRORS_CONF=/etc/kpm/kpm_mirrors.conf
# Package cache directory
TMP_LIST_FILE=/tmp/kpm/kpm_packages.list
```
Everything in capitals **must not** be changed; kpm will fail otherwise. Other than that, you are free to edit this to your liking. Lines beginning with '#' are ignored.

### INSTALL_DIR
This variable dictates where kpm will install its packages. This folder will contain:
+ Package install and uninstall scripts
+ Package info such as version, creator etc.
+ Documentation
+ etc.

### MIRRORS_CONF
This file contains all your mirrors. It is structured as a key-value file, separated by whitespace and newlines. By default, there is only one mirror:
```
https://raw.githubusercontent.com/gingrspacecadet/kpm/main/packages.list https://github.com/gingrspacecadet/kpm/releases/download/{pkg}/{pkg}.zip
```
The left value is the package list of the remote repository. It should contain all the packages available at the right value, substituting '{pkg}' for the package name. 

### `TMP_LIST_FILE`
This is a temporary, intermediary file that just stores the package list of the current repository kpm is looking through.