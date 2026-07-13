# udp-flashlc-bridge

`udp-flashlc-bridge` is a small command-line utility originally developed by **Georg Kaindl** in 2009 as part of the **Touché** multi-touch framework.

It receives UDP datagrams (typically OSC/TUIO messages) and forwards them to Adobe Flash applications using Flash Player's undocumented **LocalConnection** shared-memory protocol.

This repository preserves the original implementation, restores the build system for modern development environments, and documents the reverse engineering work that made the project possible.

---

## Supported Platforms

| Platform | Status |
|----------|--------|
| macOS Apple Silicon | ✅ |
| macOS Intel | ✅ |
| Windows x64 | ✅ |
| Windows ARM64 | ✅ |
| Linux x64 | ✅ |
| Linux ARM64 | ✅ |

---

## Project Philosophy

This project is **not a rewrite**.

The goal is to preserve the original behaviour, architecture and design while improving documentation, build tooling and long-term maintainability.

Functional changes are intentionally kept to a minimum. Wherever possible, the original implementation is preserved exactly as written.

---

## Features

- Receive UDP datagrams
- Forward data to Adobe Flash through LocalConnection
- AMF3 ByteArray serialization
- Cross-platform implementation (macOS and Windows)
- Original architecture preserved
- Modern CMake build system
- Builds cleanly with current Apple Clang
- Verified on Apple Silicon and Intel Macs

---

## Architecture

The project consists of three small, well-separated components.

```text
                 UDP Datagram
                       │
                       ▼
                UDPListener.c
                       │
                  Packet Callback
                       │
                       ▼
             TFFlashLCSHMEM.c
                       │
          Flash LocalConnection
          (Shared Memory + AMF)
                       │
                       ▼
             Adobe Flash Player
```

### Components

**udp-flashlc-bridge.c**

Application entry point.

Responsibilities:

- Parse command-line arguments
- Create the UDP listener
- Create the Flash LocalConnection context
- Forward incoming UDP packets to Flash

**UDPListener**

Encapsulates all UDP socket handling.

Responsibilities:

- Create and bind the UDP socket
- Receive datagrams
- Invoke the registered callback

The module has no knowledge of Flash or AMF.

**TFFlashLCSHMEM**

Implements Flash Player's undocumented LocalConnection transport using shared memory, semaphores and AMF serialization.

This module contains the reverse engineering work performed by Georg Kaindl.

---

## Building

### Requirements

- CMake 3.20 or newer
- A C99-compatible compiler

### Build

```bash
cmake -B build
cmake --build build
```

The original source layout has intentionally been preserved.

---

## Current Build Status

Verified working on:

- ✅ Apple Silicon (native)
- ✅ Intel macOS (native)
- ✅ macOS Catalina (Intel)
- ✅ Current Apple Clang
- ✅ CMake

The original source builds without functional modifications.

---

## Documentation

Additional documentation can be found in the `docs/` directory.

Current topics include:

- Project architecture
- Flash LocalConnection protocol
- Shared memory layout
- AMF serialization
- Connection lifecycle
- Reverse engineering notes
- Screenshots

The goal is to document the implementation thoroughly enough that it can be understood without reading the source code.

---

## Shell scripts

start without detailed logging (or via doubleclick): 
```bash
./scripts/run.sh
```
start with detailed logging (verbose): 
```bash
./scripts/run-verbose.sh
```

---

## Example Log (Verbose Mode)

```lang-sh
UDP listening port: 3333
Log Level: 3
[INFO] Listening for UDP at port 3333.
[INFO] Serving to Flash via LocalConnection at _OscDataStream:receiveOscData
[DEBUG] Received 132 bytes from 127.0.0.1:60889
[DEBUG] TUIO /tuio/2Dcur: frame=6142 source=upddcommander@192.168.0.38 alive=0 touches=0
[DEBUG] Forwarded 132 bytes to Flash.
[DEBUG] Received 132 bytes from 127.0.0.1:65188
[DEBUG] TUIO /tuio/2Dcur: frame=6150 source=upddcommander@192.168.0.38 alive=0 touches=0
[DEBUG] Forwarded 132 bytes to Flash.
[DEBUG] Received 192 bytes from 127.0.0.1:60889
[DEBUG] TUIO /tuio/2Dcur: frame=6234 source=upddcommander@192.168.0.38 alive=1 touches=1
[DEBUG]   alive session ids: 2
[DEBUG]   touch #1: session=2 pos=(0.4789, 0.5812) velocity=(0.0000, 0.0000) motionAccel=0.0000
[DEBUG] Forwarded 192 bytes to Flash.
[DEBUG] Received 376 bytes from 127.0.0.1:60889
[DEBUG] TUIO /tuio/2Dcur: frame=6235 source=upddcommander@192.168.0.38 alive=4 touches=4
[DEBUG]   alive session ids: 4, 2, 3, 5
[DEBUG]   touch #1: session=4 pos=(0.1592, 0.6415) velocity=(0.8130, -0.8023) motionAccel=50.0421
[DEBUG]   touch #2: session=2 pos=(0.5214, 0.5411) velocity=(1.7994, -1.6960) motionAccel=104.7404
[DEBUG]   touch #3: session=3 pos=(0.6672, 0.7700) velocity=(1.0403, -1.8069) motionAccel=89.6787
[DEBUG]   touch #4: session=5 pos=(0.2894, 0.6645) velocity=(1.0705, -0.9742) motionAccel=63.4248
[DEBUG] Forwarded 376 bytes to Flash.
```

---

## History

`udp-flashlc-bridge` was originally developed by **Georg Kaindl** in 2009 as part of the Touché multi-touch framework.

Touché used Adobe Flash as a frontend for interactive multi-touch applications. Communication between native code and Flash was performed through Flash Player's undocumented LocalConnection shared-memory protocol.

Although the original source code was no longer publicly available, it was recovered from a preserved archive.

This repository aims to preserve that implementation, document its internal architecture, and ensure that it remains buildable on modern systems while maintaining full compatibility with existing Flash applications.

---

## Acknowledgements

This project was originally created by **Georg Kaindl**.

The reverse engineering work required to understand Flash Player's undocumented LocalConnection shared-memory protocol and AMF message format is entirely his achievement.

The goal of this repository is to preserve, document and maintain this implementation for historical, educational and practical purposes while respecting the original architecture and design.

---

## License

The original project is licensed under the **GNU Lesser General Public License v3.0 (LGPL-3.0)**, as stated in the original source code.

Please refer to the accompanying license text for details.
