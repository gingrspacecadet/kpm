# Available KPM Packages

KPM provides access to a variety of useful packages for your Kindle. This page lists the currently available packages that can be installed with KPM.

## How to Install Packages

To install any of these packages, use the following command on your Kindle:

```bash
kpm -S packagename
```

For example, to install KindleFetch:

```bash
kpm -S kindlefetch
```

## Available Packages

Browse the list of packages you can install with KPM:

- [blockamazon](packages/blockamazon.md)
- [gnomegames](packages/gnomegames.md)
- [hello](packages/hello.md)
- [hyprpad](packages/hyprpad.md)
- [kanki](packages/kanki.md)
- [kindlefetch](packages/kindlefetch.md)
- [kwordle](packages/kwordle.md)
- [make](packages/make.md)
- [musl](packages/musl.md)

See each package page for details and installation instructions.

## Managing Installed Packages

To list all installed packages:
```bash
kpm -Ql
```

To check if a specific package is installed:
```bash
kpm -Ql packagename
```

To remove a package:
```bash
kpm -R packagename
```

To check if a package is available in configured repositories:
```bash
kpm -Qr packagename
```

To list all available packages in configured repositories:
```bash
kpm -Qr
```
