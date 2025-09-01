## Installing Packages with KPM

To install a package on your Kindle using KPM:

1. Make sure KPM is installed and up to date.
2. List available packages:
   ```bash
   kpm -Qr
   ```
3. Install a package:
   ```bash
   kpm -S <package-name>
   ```
4. Verify installation:
   ```bash
   kpm -Ql
   ```

**Notes:**
- Only one package can be installed at a time.
- Use `kpm -R <package-name>` to remove a package.
- For troubleshooting, see [Support](../support.md).
