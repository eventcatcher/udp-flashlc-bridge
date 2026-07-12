cmake_minimum_required(VERSION 3.20)

project(udp-flashlc-bridge
    VERSION 0.30.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(udp-flashlc-bridge
    src/udp-flashlc-bridge.c
    src/UDPListener.c
    src/TFFlashLCSHMEM.c
    src/TuioDecoder.c
)

target_compile_options(udp-flashlc-bridge PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(APPLE)
    target_compile_definitions(udp-flashlc-bridge PRIVATE __APPLE__)
endif()