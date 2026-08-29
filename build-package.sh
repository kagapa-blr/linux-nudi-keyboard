#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
cd "$project_dir"

case "${1:-}" in
	"") ;;
	--help|-h)
		printf '%s\n' "Usage: $0"
		printf '%s\n' '  Build, test, install, and activate Nudi for local development.'
		exit 0
		;;
	*)
		printf '%s\n' "Usage: $0" >&2
		exit 2
		;;
esac

required_packages="build-essential cmake debhelper libglib2.0-dev libgtk-3-dev libibus-1.0-dev pkg-config ibus"
missing_packages=""

run_as_root() {
	if [ "$(id -u)" -eq 0 ]; then
		"$@"
	else
		sudo "$@"
	fi
}

for package in $required_packages; do
	if ! dpkg-query -W -f='${Status}' "$package" 2>/dev/null | grep -q 'install ok installed'; then
		missing_packages="$missing_packages $package"
	fi
done

if [ -n "$missing_packages" ]; then
	if [ "$(id -u)" -ne 0 ] && ! command -v sudo >/dev/null 2>&1; then
		printf '%s\n' 'sudo is required to install missing build dependencies.' >&2
		exit 1
	fi

	printf '%s\n' "Installing missing packages:$missing_packages"
	run_as_root apt-get update
	run_as_root apt-get install -y $missing_packages
fi

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
dpkg-buildpackage -us -uc -b

version_no=$(dpkg-parsechangelog -S Version)
output_dir="$project_dir/output"
mkdir -p "$output_dir"
find "$(dirname "$project_dir")" -maxdepth 1 -type f \( \
	-name 'kannada-nudi*.deb' -o \
	-name 'kannada-nudi*.ddeb' -o \
	-name 'kannada-nudi*.buildinfo' -o \
	-name 'kannada-nudi*.changes' \
\) -exec mv -t "$output_dir" {} +
package_path="$output_dir/kannada-nudi_${version_no}_amd64.deb"
printf '%s\n' "Package written to $package_path"
if [ "$(id -u)" -eq 0 ]; then
	printf '%s\n' 'Run this script as your desktop user, without sudo.' >&2
	exit 1
fi

sudo apt install --reinstall "$package_path"

if ! ibus list-engine >/dev/null 2>&1; then
	ibus-daemon --panel disable --xim --replace --daemonize
else
	ibus restart || true
fi

i=0
while [ "$i" -lt 20 ]; do
	if ibus list-engine 2>/dev/null | grep -q '[[:space:]]nudi[[:space:]]'; then
		break
	fi
	i=$((i + 1))
	sleep 1
done
if [ "$i" -eq 20 ]; then
	printf '%s\n' 'IBus did not register Nudi in this session.' >&2
	exit 1
fi

if command -v gsettings >/dev/null 2>&1 &&
	gsettings get org.gnome.desktop.input-sources sources |
	grep -q "('ibus', 'nudi')"; then
	gsettings set org.gnome.desktop.input-sources current 0
	gsettings set org.freedesktop.ibus.general engines-order "['nudi']"
	gsettings set org.freedesktop.ibus.general preload-engines "['nudi']"
fi

i=0
while [ "$i" -lt 10 ]; do
	if ibus engine nudi >/dev/null 2>&1 && [ "$(ibus engine 2>/dev/null)" = "nudi" ]; then
		break
	fi
	i=$((i + 1))
	sleep 1
done
if [ "$i" -eq 10 ]; then
	printf '%s\n' 'Nudi was registered but could not be selected in this session.' >&2
	exit 1
fi
printf '%s\n' 'Local build, tests, installation, and IBus activation completed.'