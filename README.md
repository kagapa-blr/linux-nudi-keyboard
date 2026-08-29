# Linux Nudi Keyboard

This project ports the Kannada Nudi keyboard to Ubuntu through IBus. The engine
is implemented in C++ and IBus is a
desktop input-method service, so the engine works in applications that support
the standard Linux input-method protocol, including GTK and Qt applications on
Wayland.

## Install a downloaded package

See the end-user guide in [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for the local package installation flow. For a downloaded release, run:

```sh
sudo apt install ./ibus-nudi_1.0.0-1_amd64.deb
ibus restart
ibus list-engine | grep -i nudi
```

Add **Kannada Nudi** under **Settings > Keyboard > Input Sources**. The package
post-install script restarts IBus when a user session is available.

## Build and install the Debian package

```sh
sudo apt install debhelper-compat cmake g++ libglib2.0-dev libgtk-3-dev libibus-1.0-dev pkg-config ibus
dpkg-buildpackage -us -uc -b
sudo apt install ../ibus-nudi_1.0.0-1_amd64.deb
```

`./build-package.sh` checks these dependencies and installs any missing packages
before building. After packaging, it asks whether to start the IBus engine.

Restart the user IBus service, then open **Settings > Keyboard > Input Sources**
and add **Kannada Nudi**. Select it with the normal GNOME input-source switcher.

The engine is a user-session background process launched by IBus. No root-level
keyboard hook or key injection is used, which is required for reliable Wayland
support.

## Nudi editor

Launch the test editor after installing the package:

```sh
nudi-editor
```

The editor uses the same Nudi composer directly, shows the current composition
state, and can restart the user-session IBus engine with **Start Nudi Engine**.

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