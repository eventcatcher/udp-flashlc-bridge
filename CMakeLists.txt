cmake_minimum_required(VERSION 3.20)

project(udp-flashlc-bridge LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(udp-flashlc-bridge
    src/udp-flashlc-bridge.c
    src/UDPListener.c
    src/TFFlashLCSHMEM.c
)

target_compile_options(udp-flashlc-bridge PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
