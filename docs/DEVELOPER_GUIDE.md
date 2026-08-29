# Nudi Developer Guide

## Architecture

The project has three deliberately small layers:

- `src/nudi_composer.hpp/.cpp` contains the Unicode mapping and composition
  state machine. It has no GTK, Qt, Wayland, or IBus dependency.
- `src/ibus_engine.cpp` is the IBus adapter. It translates key events into
  composer calls, updates preedit text, and commits Unicode to the focused
  application.
- `com.example.Nudi.xml` registers the engine with the user's IBus session.

IBus is the integration boundary for ONLYOFFICE and other GTK/Qt applications.
The engine must not install a global keyboard hook or inject synthetic key
events: those approaches are unreliable or blocked under Wayland.

## Mapping source of truth

`win-nudi-keyboard.ahk` is the reference implementation. When porting a rule:

1. Record the AHK hotkey, condition, emitted code points, and state changes.
2. Add the smallest equivalent mapping/state transition in `Composer`.
3. Add a C++ assertion in `tests/test_composer.cpp` for the sequence.
4. Verify the result with code-point-aware tools, not only visual glyph output.

Important control characters are ZWNJ (`U+200C`), ZWJ (`U+200D`), and Kannada
virama (`U+0CCD`). Keep them explicit in tests because font shaping can make
different sequences look similar.

## Dependencies

### End users

End users do not need C++, CMake, headers, or development tools. They download
the `.deb` release and install it with Ubuntu's package manager:

```sh
sudo apt install ./kannada-nudi_1.0.5_amd64.deb
ibus-daemon --panel disable --xim --daemonize
```

The package declares only runtime requirements. Ubuntu resolves the linked
system libraries automatically, and `ibus` supplies the user-session service.

### Developers

Developers need the compiler, CMake, GTK3 and IBus headers, GLib headers,
packaging tools, and the IBus runtime:

```sh
sudo apt update
sudo apt install build-essential cmake debhelper-compat \
  libglib2.0-dev libgtk-3-dev libibus-1.0-dev pkg-config ibus
```

## Build and test

Install the native dependencies:

```sh
sudo apt install build-essential cmake libglib2.0-dev libgtk-3-dev \
  libibus-1.0-dev pkg-config
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Build a local package:

```sh
dpkg-buildpackage -us -uc -b
```

For a single command after dependencies are installed:

```sh
./build-package.sh
```

The package is written one directory above the repository. Inspect its runtime
dependencies before installing it:

```sh
dpkg-deb -I ../kannada-nudi_1.0.5_amd64.deb | sed -n '/Package:/p;/Version:/p;/Architecture:/p;/Depends:/p'
```

## Updating the keyboard

1. Update `src/nudi_composer.cpp` using `win-nudi-keyboard.ahk` as the source of
  truth. Keep Unicode code points explicit for ZWNJ, ZWJ, virama, and combining
  marks.
2. Add a regression assertion in `tests/test_composer.cpp` for every new key
  sequence or modifier layer.
3. Update the version in `debian/changelog` and the download version in
  `README.md` when preparing a release.
4. Run the complete local checks:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
git diff --check
```

Do not commit the `build/` directory or generated `.deb` files.

## Publishing for Ubuntu users

The repository workflow builds on Ubuntu 24.04. To publish a downloadable
release from GitHub:

```sh
git add src tests docs debian README.md CMakeLists.txt
git commit -m "Release Nudi 1.0.1"
git tag v1.0.1
git push origin main
git push origin v1.0.1
```

Pushing a `v*.*.*` tag starts `.github/workflows/package.yml`. It builds the
Debian package, uploads it as a workflow artifact, and attaches it to a GitHub
Release. Users can download that `.deb` without installing any developer
dependencies. The tag, Debian changelog version, and package filename must
match.

For a private or non-GitHub deployment, copy the generated `.deb` to a web or
APT repository and give users the same `sudo apt install ./package.deb` command.
Do not distribute the build directory or compiler dependencies to end users.

## Runtime debugging

Check that IBus sees the component:

```sh
ibus list-engine | grep -i nudi
ibus restart
```

Use `ibus engine nudi` to select it temporarily. Confirm the active desktop
session is using IBus before debugging the engine itself:

```sh
echo "$XDG_SESSION_TYPE"
echo "$GTK_IM_MODULE/$QT_IM_MODULE"
```

The package installs the engine in `/usr/libexec` and component metadata in
`/usr/share/ibus/component`. IBus launches the process on demand as a
user-session background service.

## Adding full parity

The remaining context-sensitive AHK behavior is concentrated in shifted `f`
variants and special `x` sequences. The direct CapsLock, ScrollLock, Kannada
digit, shifted-letter, vowel-sign, and core conjunct mappings are represented in
the native composer. Implement further rules as explicit composer modes instead
of reproducing the AHK global flag list. Keep
commit boundaries observable: separators should commit pending preedit and then
be passed through to the application.