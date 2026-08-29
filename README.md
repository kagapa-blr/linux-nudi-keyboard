# Kannada Nudi Linux Keyboard

This project brings the Kannada Nudi keyboard to Ubuntu through IBus. Once
installed, it works as a normal desktop input method in supported GTK and Qt
applications.

## Download and install

1. Open the latest GitHub Release.
2. Download the Debian package, for example:
   `kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb`
3. Install it:

```sh
sudo apt install ./kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
```

4. Restart IBus:

```sh
ibus restart
```

5. Confirm the engine is available:

```sh
ibus list-engine | grep -i nudi
```

You should see output similar to:

```text
nudi - Nudi Kannada
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
sudo apt install ./kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
ibus restart
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
./build-package.sh
```

`win-nudi-keyboard.ahk` remains the behavior reference. The Linux port currently
covers the base consonants, independent vowels, vowel signs, virama/conjunct
composition, backspace, and separator handling. The remaining symbol and
modifier layers should be ported as focused tests before declaring full parity.