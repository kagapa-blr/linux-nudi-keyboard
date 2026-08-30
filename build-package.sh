
#!/bin/sh

set -eu

# ============================================================
# Project directory
# ============================================================

project_dir=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
cd "$project_dir"


# ============================================================
# Configuration
# ============================================================

required_packages="
build-essential
cmake
debhelper
libglib2.0-dev
libgtk-3-dev
libibus-1.0-dev
pkg-config
ibus
"


# ============================================================
# Helpers
# ============================================================

run_as_root()
{
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    else
        sudo "$@"
    fi
}


# ============================================================
# Install missing dependencies
# ============================================================

install_dependencies()
{
    missing_packages=""

    for package in $required_packages; do

        if ! dpkg-query \
            -W \
            -f='${Status}' \
            "$package" \
            2>/dev/null |
            grep -q 'install ok installed'
        then
            missing_packages="$missing_packages $package"
        fi

    done


    if [ -n "$missing_packages" ]; then

        if [ "$(id -u)" -ne 0 ] &&
           ! command -v sudo >/dev/null 2>&1
        then
            printf '%s\n' \
                "sudo is required to install missing dependencies." >&2

            exit 1
        fi


        printf '\n%s\n' \
            "Installing missing packages:"

        printf '%s\n' \
            "$missing_packages"


        run_as_root apt-get update

        run_as_root apt-get install \
            -y \
            $missing_packages

    fi
}


# ============================================================
# Configure project
# ============================================================

configure_project()
{
    printf '\n========================================\n'
    printf '%s\n' 'Configuring Nudi'
    printf '========================================\n'

    cmake \
        -S . \
        -B build \
        -DCMAKE_BUILD_TYPE=Release
}


# ============================================================
# Build project
# ============================================================

build_project()
{
    configure_project

    printf '\n========================================\n'
    printf '%s\n' 'Building Nudi'
    printf '========================================\n'

    cmake \
        --build build \
        --parallel

    printf '\n========================================\n'
    printf '%s\n' 'Build completed successfully'
    printf '========================================\n'
}


# ============================================================
# Test project
# ============================================================

test_project()
{
    configure_project

    printf '\n========================================\n'
    printf '%s\n' 'Building test binaries'
    printf '========================================\n'

    cmake \
        --build build \
        --parallel


    printf '\n========================================\n'
    printf '%s\n' 'Running tests'
    printf '========================================\n'

    ctest \
        --test-dir build \
        --output-on-failure


    printf '\n========================================\n'
    printf '%s\n' 'All tests completed successfully'
    printf '========================================\n'
}


# ============================================================
# Create Debian package
# ============================================================

create_package()
{
    printf '\n========================================\n'
    printf '%s\n' 'Creating Debian package'
    printf '========================================\n'

    dpkg-buildpackage \
        -us \
        -uc \
        -b


    version_no=$(
        dpkg-parsechangelog \
            -S Version
    )


    output_dir="$project_dir/output"

    mkdir \
        -p \
        "$output_dir"


    find \
        "$(dirname "$project_dir")" \
        -maxdepth 1 \
        -type f \
        \( \
            -name 'kannada-nudi*.deb' -o \
            -name 'kannada-nudi*.ddeb' -o \
            -name 'kannada-nudi*.buildinfo' -o \
            -name 'kannada-nudi*.changes' \
        \) \
        -exec mv \
            -t "$output_dir" \
            {} +


    package_path="$output_dir/kannada-nudi_${version_no}_amd64.deb"


    if [ ! -f "$package_path" ]; then

        printf '%s\n' \
            "Package was not found:" >&2

        printf '%s\n' \
            "$package_path" >&2

        exit 1

    fi


    printf '\n%s\n' \
        "Package written to:"

    printf '%s\n' \
        "$package_path"
}


# ============================================================
# Install package
# ============================================================

install_package()
{
    package_path=$(find \
        "$project_dir/output" \
        -maxdepth 1 \
        -type f \
        -name 'kannada-nudi*_amd64.deb' \
        -print \
        | head -n 1)


    if [ -z "$package_path" ]; then

        printf '%s\n' \
            'Could not find the Nudi Debian package.' >&2

        exit 1

    fi


    printf '\n========================================\n'
    printf '%s\n' 'Installing Nudi'
    printf '========================================\n'


    run_as_root apt install \
        --reinstall \
        "$package_path"
}


# ============================================================
# Start / restart IBus
# ============================================================

restart_ibus()
{
    printf '\n========================================\n'
    printf '%s\n' 'Restarting IBus'
    printf '========================================\n'
        ibus-daemon --panel disable --xim --daemonize
        ibus restart

    sleep 2
}


# ============================================================
# Wait until Nudi is registered
# ============================================================

wait_for_nudi()
{
    i=0

    while [ "$i" -lt 20 ]; do

        if ibus list-engine 2>/dev/null |
            grep -q '[[:space:]]nudi[[:space:]]'
        then
            printf '%s\n' \
                'Nudi engine registered successfully.'

            return 0
        fi


        i=$((i + 1))

        sleep 1

    done


    printf '%s\n' \
        'IBus did not register Nudi in this session.' >&2

    exit 1
}


# ============================================================
# Activate Nudi
# ============================================================

activate_nudi()
{
    printf '\n========================================\n'
    printf '%s\n' 'Activating Nudi'
    printf '========================================\n'


    wait_for_nudi


    # Set Nudi as current IBus engine.
    ibus engine nudi


    i=0

    while [ "$i" -lt 10 ]; do

        current_engine=$(
            ibus engine 2>/dev/null || true
        )


        if [ "$current_engine" = "nudi" ]; then

            printf '%s\n' \
                'Nudi activated successfully.'

            return 0

        fi


        i=$((i + 1))

        sleep 1

    done


    printf '%s\n' \
        'Nudi was registered but could not be activated.' >&2

    exit 1
}


# ============================================================
# Build and Run
# ============================================================

build_and_run()
{
    if [ "$(id -u)" -eq 0 ]; then

        printf '%s\n' \
            'Please run this script as your normal desktop user, not root.' >&2

        exit 1

    fi


    install_dependencies

    build_project

    printf '\n========================================\n'
    printf '%s\n' 'Running tests before packaging'
    printf '========================================\n'

    ctest \
        --test-dir build \
        --output-on-failure


    create_package

    install_package

    restart_ibus

    activate_nudi


    printf '\n========================================\n'
    printf '%s\n' 'SUCCESS'
    printf '%s\n' 'Nudi was built, tested, installed and activated.'
    printf '========================================\n'
}


# ============================================================
# Main menu
# ============================================================

show_menu()
{
    printf '\n'
    printf '========================================\n'
    printf '        Kannada Nudi Development\n'
    printf '========================================\n'
    printf '\n'

    printf '1) Build\n'
    printf '2) Build & Run\n'
    printf '3) Test\n'
    printf '4) Exit\n'

    printf '\n'

    printf 'Select an option: '
}


# ============================================================
# Main
# ============================================================

case "${1:-}" in

    "")
        show_menu

        read -r choice

        case "$choice" in

            1)
                install_dependencies
                build_project
                ;;

            2)
                build_and_run
                ;;

            3)
                install_dependencies
                test_project
                ;;

            4)
                printf '%s\n' 'Exiting.'
                exit 0
                ;;

            *)
                printf '%s\n' \
                    'Invalid option.'

                exit 1
                ;;

        esac
        ;;


    build)
        install_dependencies
        build_project
        ;;


    run)
        build_and_run
        ;;


    test)
        install_dependencies
        test_project
        ;;


    --help|-h)

        printf '%s\n' \
            "Usage: $0 [build|run|test]"

        printf '\n'

        printf '%s\n' \
            'Without arguments, an interactive menu is displayed.'

        exit 0
        ;;


    *)

        printf '%s\n' \
            "Unknown command: $1" >&2

        printf '%s\n' \
            "Usage: $0 [build|run|test]" >&2

        exit 2
        ;;

esac

