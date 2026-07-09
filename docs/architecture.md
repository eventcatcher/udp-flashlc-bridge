Architecture

Overview

udp-flashlc-bridge is a small utility that receives UDP datagrams and forwards them to Adobe Flash applications through Flash’s LocalConnection mechanism.

The project was originally developed by Georg Kaindl as part of the Touché multi-touch framework.

The application intentionally separates networking from Flash communication. The main application contains almost no protocol-specific logic and simply connects both modules.

                UDP
                 │
                 ▼
        +----------------+
        | UDPListener    |
        +----------------+
                 │
                 ▼
        Packet Callback
                 │
                 ▼
        +----------------+
        | TFFlashLCSHMEM |
        +----------------+
                 │
                 ▼
        Flash LocalConnection
                 │
                 ▼
        Adobe Flash Player

Components

udp-flashlc-bridge.c

Application entry point.

Responsibilities:

* Parse command line arguments
* Create the UDP listener
* Create the Flash LocalConnection bridge
* Register the receive callback
* Run the application loop

The file intentionally contains almost no protocol logic.

⸻

UDPListener

Encapsulates all UDP networking.

Responsibilities:

* Create and bind the UDP socket
* Receive datagrams
* Invoke the registered callback
* Hide platform-specific socket details

The module has no knowledge of OSC, TUIO or Flash.

⸻

TFFlashLCSHMEM

Implements the Flash LocalConnection protocol using shared memory and semaphores.

Responsibilities:

* Discover the Flash shared memory segment
* Encode AMF messages
* Synchronize access to shared memory
* Deliver byte arrays to Flash applications

This module contains most of the reverse engineering work of the original project.

Design Principles

The original implementation follows a remarkably clean architecture:

* Small modules with clearly defined responsibilities
* Minimal dependencies
* Portable C implementation
* Synchronous execution model
* No unnecessary abstractions

The modernization effort aims to preserve these design principles while updating the build system and platform compatibility.


UDP Packet

      │

      ▼

TFLCSSendByteArray()

      │

      ▼

Build Envelope

      │

      ▼

Write Connection Name

      │

      ▼

Write Method Name

      │

      ▼

Serialize ByteArray

      │

      ▼

Update Header

      │

      ▼

Shared Memory

      │

      ▼

Flash Player