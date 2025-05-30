## Basic Usage

KPM provides a simple set of commands to manage packages on your Kindle. Here are the basic commands to get you started:

### Querying Available Packages

To see what packages are available in the repositories:

```bash
kpm -Qr
```

To check if a specific package is available:

```bash
kpm -Qr package-name
```

For example:

```bash
kpm -Qr kwordle
```

### Installing Packages

To install a package:

```bash
kpm -S package-name
```

For example, to install KindleFetch:

```bash
kpm -S kindlefetch
```

Note: KPM currently supports installing one package at a time.

### Listing Installed Packages

To see what packages are installed on your device:

```bash
kpm -Ql
```

To check if a specific package is installed:

```bash
kpm -Ql package-name
```

### Updating KPM 

To update KPM itself to the latest version:

```bash
kpm -S kpm
```

### Removing Packages

To remove an installed package:

```bash
kpm -R package-name
```

### Getting Help

For basic help with KPM commands:

```bash
kpm
```

For help with a specific operation (S, R, Q):

```bash
kpm -S
kpm -R
kpm -Q
```

### Command Summary

| Command | Description |
|---------|-------------|
| `kpm -S package` | Install a package |
| `kpm -R package` | Remove a package |
| `kpm -Ql` | List all installed packages |
| `kpm -Ql package` | Check if a package is installed |
| `kpm -Qr` | List all available packages |
| `kpm -Qr package` | Check if a package is available |
| `kpm remove [package]` | Remove packages |
| `kpm help` | Show help information |

For more advanced usage, check out our [Advanced Features](/advanced) guide.
