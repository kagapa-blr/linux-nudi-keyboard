# Kannada Nudi Linux Keyboard

This project brings the Kannada Nudi keyboard to Ubuntu through IBus. Once
installed, it works as a normal desktop input method in supported GTK and Qt
applications.

## Download and install

1. Open the latest GitHub Release.
2. Download the package for your distribution:
	- Debian, Ubuntu, and Linux Mint: `kannada-nudi_1.0.0_amd64.deb`
	- Fedora and other RPM-based distributions: `kannada-nudi-1.0.0_x86_64.rpm`
3. If an older `kannada-nudi` package is still installed, remove it first:

```sh
sudo apt purge -y kannada-nudi
```

4. Install it:

```sh
sudo apt install ./kannada-nudi_1.0.0_amd64.deb
```

On Fedora or another RPM-based distribution, install the RPM with:

```sh
sudo dnf install ./kannada-nudi-1.0.0_x86_64.rpm
```

4. Ensure the user IBus daemon is running:

```sh
ibus-daemon --panel disable --xim --daemonize
```

If you already have a session running, you can also use:

```sh
ibus restart
```

Select Nudi in the current IBus session:

```sh
ibus engine nudi
```

5. Confirm the engine is available:

```sh
ibus list-engine | grep -i nudi
```

You should see output similar to:

```text
nudi - Nudi Kannada
```

## Start and stop Nudi

Start the IBus session and select Nudi:

```sh
ibus-daemon --panel disable --xim --daemonize
ibus engine nudi
```

To stop using Nudi, switch back to your normal keyboard layout from the
desktop input-source switcher. To stop the IBus session completely:

```sh
ibus exit
```

Runtime diagnostics are written to `/var/log/kannada-nudi/YYYY-MM-DD/`.
Each desktop user has a separate `engine-UID.log` file; the logs record engine
activity and key metadata, but not composed Kannada text.

To inspect the latest log:

```sh
ls -ltr /var/log/kannada-nudi/"$(date +%F)"
tail -f /var/log/kannada-nudi/"$(date +%F)"/engine-"$(id -u)".log
```

## Use it in Ubuntu

Open:

Settings → Keyboard → Input Sources

Then:

1. Click the `+` button
2. Choose `Kannada`
3. Select `Nudi Kannada`
4. Start typing in Kannada

This works in supported Linux apps as a standard IBus input method.

## Local package install

If you are testing a local build instead of a GitHub release, install it the same way:

```sh
sudo apt install ./kannada-nudi_1.0.0_amd64.deb
ibus-daemon --panel disable --xim --daemonize
```

## Launch the built-in editor

You can also launch the included editor for testing the keyboard logic directly:

```sh
nudi-editor
```

The editor shows the current composition state and helps validate keyboard behavior.

For the complete user walkthrough and troubleshooting steps, see [docs/USER_GUIDE.md](docs/USER_GUIDE.md).

## Developer build

Install build dependencies and build the package:

```sh
sudo apt install build-essential cmake debhelper-compat \
	libglib2.0-dev libgtk-3-dev libibus-1.0-dev pkg-config ibus
./build-linux-package.sh
```

The script also supports the following development commands:

```sh
./build-linux-package.sh test       # build from the current tree and run CTest
./build-linux-package.sh clean      # remove build/ and output/
./build-linux-package.sh uninstall  # purge the installed Debian package
./build-linux-package.sh reinstall  # uninstall, clean, rebuild, test, install
```

The `test` command runs every test registered with CTest, including the
composer tests and the IBus registration contract check.

On Windows, use PowerShell to build and test the console executable:

```powershell
.\build-windows.ps1 -Configuration Release -Clean
```

The Windows script writes `build-artifacts/kannada-nudi-windows-Release.zip`,
containing `nudi-windows.exe` and the Windows composer test executable.

`win-nudi-keyboard.ahk` remains the behavior reference. The Linux port currently
covers the base consonants, independent vowels, vowel signs, virama/conjunct
composition, backspace, and separator handling. The remaining symbol and
modifier layers should be ported as focused tests before declaring full parity.