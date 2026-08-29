#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
cd "$project_dir"

install_package=false
case "${1:-}" in
	"") ;;
	--install) install_package=true ;;
	--help|-h)
		printf '%s\n' "Usage: $0 [--install]"
	printf '%s\n' '  --install  build, install the package, and refresh the user IBus session'
		exit 0
		;;
	*)
		printf '%s\n' "Usage: $0 [--install]" >&2
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
dpkg-buildpackage -us -uc -b

version_no=$(dpkg-parsechangelog -S Version)
package_path=$(dirname "$project_dir")/kannada-nudi_${version_no}_amd64.deb
release_package=$(dirname "$project_dir")/kannada-nudi_${version_no}_amd64.deb
if [ -f "$package_path" ] && [ "$package_path" != "$release_package" ]; then
    cp "$package_path" "$release_package"
fi
printf '%s\n' "Package written to $release_package"
if "$install_package"; then
	"$project_dir/install.sh" "$release_package"
else
	printf '%s\n' "Install it with: sudo apt install $release_package"
	printf '%s\n' 'Then run install.sh or restart IBus as the desktop user and select Kannada Nudi.'
fi