@echo off
setlocal enabledelayedexpansion

REM === Ask for package name
set /p pkgname=Enter package name (e.g. hello):

REM === Paths
set pkgdir=packages\%pkgname%
set zipfile=%pkgname%.zip

REM === Check if folder exists
if not exist %pkgdir% (
    echo ERROR: Package folder "%pkgdir%" not found!
    exit /b 1
)

REM === Remove old zip if exists
if exist %zipfile% (
    del %zipfile%
)

REM === Zip the package
echo Zipping %pkgdir% into %zipfile%...
powershell Compress-Archive -Path %pkgdir%\* -DestinationPath %zipfile%
if %errorlevel% neq 0 (
    echo ERROR: Failed to zip package
    exit /b 1
)

REM === Create release tag (if it doesn't exist)
echo Creating tag and GitHub release "%pkgname%"...
gh release create %pkgname% %zipfile% --title "%pkgname%" --notes "Auto-uploaded package: %pkgname%" --latest
if %errorlevel% neq 0 (
    echo ERROR: Failed to create release
    exit /b 1
)

echo Package "%pkgname%" uploaded to GitHub release.
rm %zipfile%
endlocal
pause
