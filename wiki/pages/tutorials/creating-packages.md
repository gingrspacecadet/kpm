## Creating Packages for KPM

This tutorial will guide you through the process of creating your own packages for KPM. By creating packages, you can share your applications or scripts with other Kindle users in a standardized way.

### Package Structure

A basic KPM package consists of:

1. Your application files
2. An installation script (`install.sh`)
3. An uninstallation script (`uninstall.sh`)

KPM packages are typically distributed as ZIP archives containing these components.

### Creating a Basic Package

Follow these steps to create a basic package:

#### 1. Create a Directory Structure

```
my-package/
├── install.sh
├── uninstall.sh
└── your-files/
    ├── executable
    ├── data-files
    └── etc.
```

The `install.sh` script handles installation of your package, while `uninstall.sh` handles removal.

#### 2. Write the Installation Script

Create an `install.sh` script that will handle the installation of your package:

```bash
#!/bin/sh
# Example install.sh for KPM package

# Make the installation directory within the KPM packages location
mkdir -p /mnt/us/kpm/packages/bin

# Copy your executable to the packages bin directory
cp /mnt/us/kpm/packages/my-package/my-executable /mnt/us/kpm/packages/bin/

# Make it executable
chmod +x /mnt/us/kpm/packages/bin/my-executable

# Copy any additional files needed
mkdir -p /mnt/us/my-package/data
cp -r /mnt/us/kpm/packages/my-package/data/* /mnt/us/my-package/data/

echo "My package installed successfully!"
```

#### 3. Write the Uninstallation Script

Create an `uninstall.sh` script that will handle removal of your package:

```bash
#!/bin/sh
# Example uninstall.sh for KPM package

# Remove the executable
rm /mnt/us/kpm/packages/bin/my-executable

# Remove any additional files
rm -rf /mnt/us/my-package

echo "My package uninstalled successfully!"
```

#### 4. Make Scripts Executable

Make sure both scripts are executable:

```bash
chmod +x install.sh uninstall.sh
```

#### 5. Create the Package Archive

Create a ZIP archive containing your package:

```bash
zip -r my-package.zip install.sh uninstall.sh my-executable data/
```

### Adding Your Package to a Repository

To make your package available to other users:

1. Host the ZIP file on a web server or GitHub releases
2. Create a package list file that includes your package name
3. Set up a mirror configuration that points to your package repository

Example mirror configuration format:
```
https://your-server.com/packages.conf https://your-server.com/packages/{pkg}/{pkg}.zip
```

Where:
- The first URL is the package list file
- The second URL is the package download format with `{pkg}` as a placeholder

#### Best Practices

1. Keep your package small and focused
2. Test your installation and uninstallation scripts thoroughly
3. Make sure all files have appropriate permissions
4. Use the standard directory structure when possible:
   - Executables go in `/mnt/us/kpm/packages/bin/`
   - Data files go in `/mnt/us/your-package/`
5. Include a simple README or help command

#### Example Package

See the [hello](/packages/hello) package for a simple example of a KPM package.

### Testing Your Package

To test your package:

1. Build it as described above
2. Install it on your device:
   ```bash
   kpm -U ./my-package_1.0.0.zip
   ```
3. Test all the functionality
4. Uninstall it to make sure removal works correctly:
   ```bash
   kpm -R my-package
   ```

### Publishing Your Package

To share your package with others:

1. Host your `.zip` file on a web server or GitHub as well as a package list
2. Create a repository index file listing your packages
3. Share the repository URL so others can add it to their KPM configuration:
   ```bash
   kpm add-repository http://your-repo-url.com/
   ```

For more detailed information on package creation, see the [Package Development Guide](/package-development).
