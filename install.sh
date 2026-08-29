#!/bin/sh
set -eu

if [ "$(id -u)" -eq 0 ]; then
    echo 'Run this script as your desktop user, without sudo.' >&2
    echo 'The script uses sudo only for package installation.' >&2
    exit 1
fi

package=${1:-}
if [ -z "$package" ]; then
    package=$(find "$(dirname "$0")/.." -maxdepth 1 -type f -name 'kannada-nudi_*.deb' -print -quit)
fi
if [ -z "$package" ] || [ ! -f "$package" ]; then
    echo "Usage: $0 /path/to/kannada-nudi_1.0.5_amd64.deb" >&2
    exit 2
fi

if dpkg-query -W -f='${Status}' ibus-nudi 2>/dev/null | grep -q 'install ok installed'; then
    echo 'Removing legacy ibus-nudi package before installing the new package.' >&2
    sudo apt purge -y ibus-nudi
fi

if dpkg-query -W -f='${Status}' kannada-nudi-linux-keyboard 2>/dev/null | grep -q 'install ok installed'; then
    echo 'Removing legacy kannada-nudi-linux-keyboard package before installing the new package.' >&2
    sudo apt purge -y kannada-nudi-linux-keyboard
fi

if dpkg-query -W -f='${Status}' kannada-nudi 2>/dev/null | grep -q 'install ok installed'; then
    echo 'Removing existing kannada-nudi package before reinstalling the new package.' >&2
    sudo apt purge -y kannada-nudi
fi

sudo apt install --reinstall "$package"
restart_ibus() {
    if command -v ibus >/dev/null 2>&1 && ibus list-engine >/dev/null 2>&1; then
        return 0
    fi
    if command -v ibus-daemon >/dev/null 2>&1; then
        ibus-daemon --panel disable --xim --daemonize
        return 0
    fi
    if systemctl --user restart org.freedesktop.IBus.session.GNOME.service >/dev/null 2>&1; then
        return 0
    fi
    command -v ibus >/dev/null 2>&1 && ibus restart
}

wait_for_ibus() {
    i=0
    while [ "$i" -lt 20 ]; do
        if ibus list-engine 2>/dev/null | grep -q '[[:space:]]nudi[[:space:]]'; then
            return 0
        fi
        i=$((i + 1))
    done
    return 1
}

if restart_ibus; then
    if wait_for_ibus; then
        printf '%s\n' 'IBus is running.'
    else
        printf '%s\n' 'IBus did not become ready in this session.' >&2
        exit 1
    fi
    if command -v gsettings >/dev/null 2>&1 &&
        gsettings get org.gnome.desktop.input-sources sources |
        grep -q "('ibus', 'nudi')"; then
        gsettings set org.gnome.desktop.input-sources current 0
        gsettings set org.freedesktop.ibus.general engines-order "['nudi']"
        gsettings set org.freedesktop.ibus.general preload-engines "['nudi']"
    fi
    if ibus engine nudi >/dev/null 2>&1; then
        printf '%s\n' 'Nudi Kannada selected.'
    else
        printf '%s\n' 'Add Nudi Kannada in Settings > Keyboard > Input Sources, then select it.'
    fi
else
    printf '%s\n' 'IBus could not be restarted in this session.'
    printf '%s\n' 'Run: ibus restart'
fi
printf '%s\n' 'Verify with: ibus list-engine | grep -i nudi'
printf '%s\n' 'Then add Nudi Kannada in Settings > Keyboard > Input Sources.'
printf '%s\n' 'Open the visible test editor with: nudi-editor'