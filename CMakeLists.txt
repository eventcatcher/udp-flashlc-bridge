cmake_minimum_required(VERSION 3.20)

project(
    udp-flashlc-bridge
    VERSION 0.30.0
    DESCRIPTION "UDP to Adobe Flash LocalConnection bridge"
    LANGUAGES C
)

#
# C standard
#
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#
# Generate compile_commands.json
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Standard install directories
#
include(GNUInstallDirs)

#
# Executable
#
add_executable(udp-flashlc-bridge
    src/udp-flashlc-bridge.c
    src/UDPListener.c
    src/TFFlashLCSHMEM.c
    src/TuioDecoder.c
)

#
# Include directories
#
target_include_directories(udp-flashlc-bridge
    PRIVATE
        src
)

#
# Compiler warnings
#
if(MSVC)

    target_compile_options(udp-flashlc-bridge PRIVATE
        /W4
    )

else()

    target_compile_options(udp-flashlc-bridge PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

#
# Platform specific definitions
#
if(APPLE)

    target_compile_definitions(udp-flashlc-bridge PRIVATE
        __APPLE__
    )

endif()

#
# POSIX feature macros
#
if(UNIX AND NOT APPLE)

    target_compile_definitions(udp-flashlc-bridge PRIVATE
        _POSIX_C_SOURCE=200809L
    )

endif()

#
# Platform specific libraries
#
if(WIN32)

    target_link_libraries(udp-flashlc-bridge PRIVATE
        ws2_32
    )

endif()

#
# Threads (reserved for future use)
#
if(UNIX)

    find_package(Threads)

    if(Threads_FOUND)
        target_link_libraries(udp-flashlc-bridge PRIVATE
            Threads::Threads
        )
    endif()

endif()

#
# Installation
#
install(
    TARGETS udp-flashlc-bridge
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#
# Testing (prepared for future decoder tests)
#
include(CTest)
