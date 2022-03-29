cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(d0g C)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# Define folders
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# Compiler settings
set(CMAKE_C_STANDARD 11)

# Includes
include_directories(${INC_DIR})
include_directories(${EXT_DIR})

# Binaries
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} m)

install(TARGETS d0g DESTINATION d0g)

include(CPack)
