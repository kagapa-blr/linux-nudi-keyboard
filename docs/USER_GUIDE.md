# Nudi Kannada User Guide

This guide explains how to install the local Debian package for Kannada Nudi and use it as a desktop input method on Ubuntu.

## 1. Download the package

From your local project folder, or from the release location you are using for testing, download the package file.

Example local package name:

```sh
ibus-nudi_1.0.0-1_amd64.deb
```

Keep the `.deb` file in a folder you can access easily, such as your Downloads folder or the project directory.

## 2. Install the package

Open a terminal and install it with:

```sh
sudo apt install ./ibus-nudi_1.0.0-1_amd64.deb
```

If you are installing from a different directory, use the full path instead:

```sh
sudo apt install /path/to/ibus-nudi_1.0.0-1_amd64.deb
```

## 3. Restart IBus

After installation, restart the input-method service:

```sh
ibus restart
```

You can confirm that the engine is available:

```sh
ibus list-engine | grep -i nudi
```

Expected output:

```text
nudi - Nudi Kannada
```

## 4. Enable it in Ubuntu

Open:

Settings → Keyboard → Input Sources

Then:

1. Click the plus button
2. Choose Kannada
3. Select Nudi Kannada from the list
4. Confirm the new input source is active

You can then switch input sources with the normal GNOME keyboard layout switcher.

## 5. Start typing in Kannada

Once selected, you can type Kannada in any application that supports standard Linux input methods, including many GTK and Qt apps.

The engine is designed to work as a regular IBus input method and does not rely on a root-level keyboard hook.

## 6. Launch the built-in test editor

The project also includes a small editor for testing the Kannada composer directly:

```sh
nudi-editor
```

This is useful for checking composition behavior and verifying that the engine responds correctly before using it in other apps.

## 7. Troubleshooting

If the input method does not appear:

```sh
ibus restart
ibus list-engine | grep -i nudi
```

Then check the desktop input sources again and add Nudi Kannada manually.

If it still does not appear, reinstall the package:

```sh
sudo apt install --reinstall ./ibus-nudi_1.0.0-1_amd64.deb
ibus restart
```

## 8. Notes

This guide uses the local `.deb` package flow for development and testing. When you are ready to distribute it publicly, replace the local download step with the online release URL.
