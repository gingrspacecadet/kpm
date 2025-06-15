## Installing KPM

This guide will walk you through the process of installing KPM on your Kindle device.

### Prerequisites

Before installing KPM, make sure you have the following:

- **Jailbroken Kindle**: KPM requires a jailbroken Kindle device
- **KTerm**: Terminal emulator for Kindle
- **curl**: For downloading files (usually comes pre-installed with KTerm)
- **unzip** and/or **tar**: For extracting packages

### Installation Steps

1. Open KTerm on your Kindle device
2. Run the following command:

```bash
sh -c "$(curl -fsSL https://raw.githubusercontent.com/gingrspacecadet/kpm/main/install-kpm.sh)"
```

3. The installation script will download the latest version of KPM and set it up on your device
4. Once the installation is complete, you can verify it by running:

```bash
kpm --version
```

### What the Installation Script Does

The installation script performs the following tasks:

1. Makes the root filesystem writable (`mntroot rw`)
2. Creates necessary directories
3. Downloads the KPM binary to `/usr/local/bin/kpm`
4. Sets up the package list at `/mnt/us/kpm/package_list.conf`
5. Creates configuration files in `/etc/kpm/`
6. Creates the package directory at `/mnt/us/kpm/packages/`
7. Adds KPM's bin directory to your PATH

### Directory Structure

After installation, KPM will create the following directory structure:

```
/usr/local/bin/kpm               # KPM binary
/etc/kpm/                        # Configuration files
├── kpm.conf                     # Main configuration
└── kpm_mirrors.conf             # Mirror URLs
/mnt/us/kpm/                     # KPM data directory
├── packages.list                # List of installed packages
└── packages/                    # Packages directory
    └── bin/                     # Binaries from packages
```

### Troubleshooting

If you encounter any issues during installation:

- Make sure your Kindle is connected to the internet
- Verify that `curl` is properly installed (should come with KTerm)
- Check if you have sufficient storage space (at least 5MB free)
- Make sure you're running the installation script as root
- Ensure the root filesystem is mounted as read-write (`mntroot rw`)

Common issues:

1. **"Permission denied" errors**:
   - Run `mntroot rw` before installation to make the filesystem writable
   - Make sure you're running the script as root

2. **"Command not found" after installation**:
   - Try restarting KTerm or logging out and back in
   - Check if `/mnt/us/kpm/packages/bin` is in your PATH

If you continue to have problems, please visit our [Support](/support) page for additional help.
