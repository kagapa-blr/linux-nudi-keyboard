cmake_minimum_required(VERSION 3.16)

project(kannada-nudi VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Dependencies
# ============================================================

find_package(PkgConfig REQUIRED)
pkg_check_modules(IBUS REQUIRED IMPORTED_TARGET ibus-1.0)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)




# ============================================================
# Nudi Composer
# ============================================================
add_library(nudi-composer STATIC src/nudi_composer.cpp)
target_include_directories(nudi-composer PUBLIC src)



# ============================================================
# IBus Engine
# ============================================================
add_executable(ibus-engine-nudi src/ibus_engine.cpp)
target_link_libraries(ibus-engine-nudi PRIVATE nudi-composer PkgConfig::IBUS)


# ============================================================
# Nudi Editor
# ============================================================

add_executable(nudi-editor src/nudi_editor.cpp)

target_link_libraries(
    nudi-editor

    PRIVATE

    nudi-composer
    PkgConfig::GTK3
)


# ============================================================
# Installation
# ============================================================

include(GNUInstallDirs)

install(
    TARGETS ibus-engine-nudi

    RUNTIME DESTINATION libexec
)

install(
    TARGETS nudi-editor

    RUNTIME DESTINATION bin
)

install(
    FILES com.example.Nudi.xml

    DESTINATION share/ibus/component
)


# ============================================================
# Tests
# ============================================================

enable_testing()


# ============================================================
# Composer test executable
# ============================================================

add_executable(
    test-composer

    tests/nudi_composer_test.cpp
    tests/nudi_test_cases.cpp
)


# ============================================================
# Test include directory
# ============================================================

target_include_directories(
    test-composer

    PRIVATE

    tests
)


# ============================================================
# Test output directory
#
# Results:
#
# project/
# └── output/
#     └── nudi-test-results.txt
# ============================================================

set(
    NUDI_TEST_OUTPUT_DIR
    "${CMAKE_SOURCE_DIR}/output"
)


# ============================================================
# Create output directory during CMake configuration
# ============================================================

file(
    MAKE_DIRECTORY
    "${NUDI_TEST_OUTPUT_DIR}"
)


# ============================================================
# Pass output directory to C++
#
# This becomes:
#
# #define NUDI_TEST_OUTPUT_DIR "/path/to/project/output"
# ============================================================

target_compile_definitions(
    test-composer

    PRIVATE

    NUDI_TEST_OUTPUT_DIR="${NUDI_TEST_OUTPUT_DIR}"
)


# ============================================================
# Link composer to test
# ============================================================

target_link_libraries(
    test-composer

    PRIVATE

    nudi-composer
)


# ============================================================
# CTest registration
# ============================================================

add_test(
    NAME composer

    COMMAND
        test-composer
)


# ============================================================
# IBus registration test
# ============================================================

add_test(
    NAME ibus-registration

    COMMAND
        sh
        ${CMAKE_SOURCE_DIR}/tests/check_ibus_registration.sh
)