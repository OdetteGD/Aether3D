cmake_minimum_required(
    VERSION 3.22.1
)



project(
    aether3d_physics
    LANGUAGES CXX
)



set(
    CMAKE_CXX_STANDARD
    20
)



set(
    CMAKE_CXX_STANDARD_REQUIRED
    ON
)



set(
    CMAKE_CXX_EXTENSIONS
    OFF
)



add_library(
    aether3d_physics
    SHARED

    native-lib.cpp
)



target_compile_options(
    aether3d_physics
    PRIVATE

    -Wall

    -Wextra

    -Wpedantic

    -O3

    -ffunction-sections

    -fdata-sections

    -ffp-contract=off
)



target_link_options(
    aether3d_physics
    PRIVATE

    -Wl,--gc-sections
)



find_library(
    android-log
    log
)



target_link_libraries(
    aether3d_physics
    PRIVATE

    ${android-log}
)
