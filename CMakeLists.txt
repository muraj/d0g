cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(d0g C)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# Define folders
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)

# Compiler settings
set(CMAKE_C_STANDARD 11)

# Includes
include_directories(${INC_DIR})

# Binaries
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} m)
