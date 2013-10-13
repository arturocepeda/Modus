cmake_minimum_required(VERSION 2.8)

project(modus)

# Define the build directory structure
set(MODUS_BUILD_DIR ${CMAKE_BINARY_DIR}/root)
set(MODUS_LIBRARY_DIR ${MODUS_BUILD_DIR}/lib)
set(MODUS_BINARY_DIR ${MODUS_BUILD_DIR}/bin)

add_subdirectory(modus)
