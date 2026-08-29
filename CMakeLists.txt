cmake_minimum_required(VERSION 3.16)
project(kannada-nudi VERSION 1.0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IBUS REQUIRED IMPORTED_TARGET ibus-1.0)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

add_library(nudi-composer STATIC src/nudi_composer.cpp)
target_include_directories(nudi-composer PUBLIC src)

add_executable(ibus-engine-nudi src/ibus_engine.cpp)
target_link_libraries(ibus-engine-nudi PRIVATE nudi-composer PkgConfig::IBUS)

add_executable(nudi-editor src/nudi_editor.cpp)
target_link_libraries(nudi-editor PRIVATE nudi-composer PkgConfig::GTK3)

include(GNUInstallDirs)
install(TARGETS ibus-engine-nudi RUNTIME DESTINATION libexec)
install(TARGETS nudi-editor RUNTIME DESTINATION bin)
install(FILES com.example.Nudi.xml DESTINATION share/ibus/component)

enable_testing()
add_executable(test-composer tests/test_composer.cpp)
target_link_libraries(test-composer PRIVATE nudi-composer)
add_test(NAME composer COMMAND test-composer)

add_test(NAME ibus-registration
    COMMAND sh ${CMAKE_SOURCE_DIR}/tests/check_ibus_registration.sh)