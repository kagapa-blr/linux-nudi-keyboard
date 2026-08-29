# Kannada Nudi Linux Keyboard User Guide

This guide explains how to download, install, enable, and use the Kannada Nudi keyboard on Ubuntu Linux.

## 1. Download the package

Go to the project's GitHub Releases page and download the latest Debian package, for example:

```sh
kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
```

You can also use a local `.deb` file if you are testing a build on your machine.

## 2. Install the package

Open a terminal and install the downloaded file:

```sh
sudo apt install ./kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
```

If the file is in another folder, use the full path instead:

```sh
sudo apt install /path/to/kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
```

## 3. Restart IBus

After installing, restart the input method service:

```sh
ibus restart
```

Check that the engine is available:

```sh
ibus list-engine | grep -i nudi
```

You should see something like:

```text
nudi - Nudi Kannada
```

## 4. Enable the keyboard in Ubuntu

Open:

Settings → Keyboard → Input Sources

Then:

1. Click the `+` button
2. Choose `Kannada`
3. Select `Nudi Kannada`
4. Confirm it is active

You can now switch keyboard layouts with the normal GNOME input-source switcher.

## 5. Start typing in Kannada

Once `Nudi Kannada` is selected, you can type in Kannada in supported applications such as:

- GTK applications
- Qt applications
- many text editors and browser fields

The keyboard works as a standard IBus input method and does not require a global keyboard hook.

## 6. Open the built-in test editor

You can also launch the included editor for testing the composer directly:

```sh
nudi-editor
```

This is useful for checking composition behavior before typing in other apps.

## 7. Troubleshooting

If the input method is not visible:

```sh
ibus restart
ibus list-engine | grep -i nudi
```

Then add it again from Settings → Keyboard → Input Sources.

If it still does not appear, reinstall the package:

```sh
sudo apt install --reinstall ./kannada-nudi-linux-keyboard_1.0.0-1_amd64.deb
ibus restart
```

## 8. How to stop using it

To stop using Nudi temporarily:

- switch back to your normal keyboard layout in Settings → Keyboard → Input Sources

To fully stop the IBus session:

```sh
ibus exit
```

Then start it again with:

```sh
ibus restart
```

## 9. Notes

This project is released as a Debian package for Ubuntu. The latest release asset is the easiest way to install and use it. The local `.deb` flow is mainly for development and testing.
