## Contributing to KPM

We welcome contributions to KPM! This page outlines how you can help make KPM even better.

### Ways to Contribute

There are many ways to contribute to KPM:

1. **Code Contributions**: Help improve the KPM codebase
2. **Documentation**: Help improve or translate the documentation
3. **Package Creation**: Create and maintain packages
4. **Bug Reports**: Report issues you encounter
5. **Feature Requests**: Suggest new features or improvements
6. **Community Support**: Help other users in the forums or chat

### Development Setup

To set up a development environment for KPM:

1. Clone the repository:
   ```bash
   git clone https://github.com/gingrspacecadet/kpm.git
   cd kpm
   ```

2. KPM is written in C and uses standard libraries. You'll need:
   - GCC
   - Make
   - An ARMv7 environment (more on that later)
   - Basic development tools

3. Build KPM:
   ```bash
   make local
   ```

This will build the `kpm` binary that you can test locally.
If you plan on creating binaries for kpm, run this script:
```bash
./emulate-download.sh
```
It will install a virtual environment similar to the kindle for you to compile binaries in. Enter it with 
```bash
./enter-kindle.sh
```

### Pull Request Workflow

1. Fork the repository on GitHub
2. Create a new branch for your feature or bugfix
3. Make your changes and commit them
4. Push your branch to your fork
5. Create a pull request from your branch to the main repository

### Contributing New Packages

To add a new package to KPM:

1. Create a new directory in the `packages` folder with your package name
   ```bash
   mkdir -p packages/my-package
   ```

2. Create the necessary files:
   - `install.sh`: Installation script
   - `uninstall.sh`: Uninstallation script
   - Any other files needed by your package

3. Make sure your scripts are executable:
   ```bash
   chmod +x packages/my-package/install.sh packages/my-package/uninstall.sh
   ```

4. Add your package name to `packages.conf`

5. Create documentation for your package in the wiki:
   ```bash
   touch wiki/pages/packages/my-package.md
   ```

6. Submit a pull request with your changes

### Coding Standards

- Use 4 spaces for indentation
- Follow the existing code style
- Try not to need comments by writing clear code
- Write tests for new features
- Update documentation for user-facing changes

### Documentation Contributions

The KPM documentation is written in Markdown and stored in the `wiki/pages` directory. To contribute:

1. Edit the appropriate Markdown files
2. Preview your changes locally using a Markdown previewer
3. Submit a pull request with your changes

### Creating and Maintaining Packages

If you're interested in creating or maintaining packages:

1. Follow the [Creating Packages](/tutorials/creating-packages) guide
2. Test your packages thoroughly
3. Submit them to the official KPM repository or host your own

### Community Guidelines

- Be respectful and kind to other contributors
- Provide constructive feedback
- Help newcomers get started
- Follow the code of conduct

### Recognition

All contributors are recognized on our [Contributors page](/contributors) and on the GitHub repository. We appreciate every contribution, no matter how small!