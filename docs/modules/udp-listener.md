# UDPListener

## Responsibilities

The UDPListener module encapsulates all UDP socket handling.

It is responsible for:

- creating the UDP socket
- binding to the requested port
- receiving datagrams
- forwarding received packets through a callback

The module intentionally contains no knowledge about OSC, TUIO or Flash.

## Design

The listener is synchronous.

The application blocks in recvfrom() until a packet arrives.

No additional threads are created.

## Public API

UDPListenerCreate()

UDPListenerCreateWithPort()

UDPListenerWaitForPacket()

UDPListenerRelease()

UDPListenerSetPacketReceiptCallback()

## Notes

The receive buffer is allocated lazily during the first receive operation and reused for all subsequent packets.