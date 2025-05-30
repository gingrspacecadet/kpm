## Installing Packages with KPM

This tutorial will guide you through the process of finding and installing packages using KPM.

### Popular Packages Available

KPM offers a variety of useful packages for your Kindle, including:

- [blockamazon](/packages/blockamazon): Block Amazon services on your Kindle
- [gnomegames](/packages/gnomegames): Classic GNOME games for Kindle
- [kindlefetch](/packages/kindlefetch): System information display
- [kwordle](/packages/kwordle): Wordle game for Kindle
- [kanki](/packages/kanki): Flashcard app for language learning

For a complete list of available packages and detailed descriptions, see the [Available Packages](/packages) page.

### Finding Available Packages

Before installing a package, you might want to see what's available in the repositories:

To list all available packages:

```bash
kpm -Qr
```

This will display all packages available from configured mirrors.

### Checking if a Package is Available

To check if a specific package is available:

```bash
kpm -Qr package-name
```

For example:

```bash
kpm -Qr kindlefetch
```

This will check all configured mirrors for the package.

### Installing a Package

To install a package:

```bash
kpm -S package-name
```

For example:

```bash
kpm -S kindlefetch
```

During the installation process:
1. KPM will download the package from the mirror
2. Extract the files to the appropriate locations (usually in `/mnt/us/kpm/packages/`)
3. Run the package's install.sh script
4. Update the installed package list

### What Happens During Installation

When you install a package with KPM, the following steps occur:

1. KPM checks if the package is already installed
2. It looks for the package in the configured mirrors
3. If found, it downloads the package archive (usually a ZIP file)
4. The archive is extracted to `/mnt/us/kpm/packages/package-name/`
5. The `install.sh` script from the package is executed
6. The package name is added to the installed package list

### Verifying Installation

To verify that a package is installed:

```bash
kpm -Ql package-name
```

For example:

```bash
kpm -Ql kindlefetch
```

### Running Installed Applications

Most packages will install executables to `/mnt/us/kpm/packages/bin/`, which should be in your PATH. This means you can simply run the commands directly from KTerm.

For example, after installing kindlefetch:

```bash
kindlefetch
```

If you get a "command not found" error, you may need to restart KTerm or add `/mnt/us/kpm/packages/bin` to your PATH manually.

### Removing Packages

To remove an installed package:

```bash
kpm -R package-name
```

For example:

```bash
kpm -R kindlefetch
```

This will run the package's uninstall.sh script and remove it from the installed package list.

### Troubleshooting Common Installation Issues

If you encounter problems during installation:

1. **Package not found**: Check that you spelled the package name correctly and that the package is available in the configured mirrors
2. **Download failures**: Ensure your Kindle is connected to the internet
3. **Installation script errors**: Check if the package's install.sh script has any specific requirements
4. **Permission issues**: Make sure you have sufficient permissions to write to the installation directories
3. **Insufficient space**: Free up space on your device
4. **Network problems**: Check your Kindle's network connection

For additional help with specific packages, refer to our [Package Reference](/packages) section.
