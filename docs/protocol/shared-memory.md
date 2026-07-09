## Known Constants

Some values used by Flash's LocalConnection implementation are not publicly documented.

The bridge therefore relies on values obtained through reverse engineering.

## Shared Memory Layout

The Flash Player allocates a fixed-size shared memory segment for its LocalConnection implementation.

The bridge relies on several constants that were obtained through reverse engineering of Flash Player.

Constant	Value	Description
FLASHLC_SHM_SIZE	64528	Total size of the shared memory segment.
FLASHLC_SHM_LISTENERS_OFFSET	40976	Offset at which the listener table begins.
MAX_LISTENER_NAME_LEN	64	Maximum LocalConnection name length.
MAX_LISTENER_METHOD_NAME	64	Maximum method name length.

The exact internal layout of the shared memory segment is not publicly documented and is reconstructed from the original implementation.

## Listener Table

The shared memory segment contains a listener table beginning at FLASHLC_SHM_LISTENERS_OFFSET.

The table consists of consecutive null-terminated strings and is terminated by an empty string.

Inspection of the original implementation shows that each registered LocalConnection occupies three consecutive string entries.

The first string is the LocalConnection name used by Flash applications.

The exact meaning of the remaining two strings requires further analysis of the Flash protocol and is currently under investigation.

## Message Layout

Each message begins with a fixed 16-byte envelope.

| Offset | Size | Purpose |
|-------:|-----:|---------|
| 0 | 4 | Flash control flag |
| 4 | 4 | Flash control flag |
| 8 | 4 | Timestamp (milliseconds) |
| 12 | 4 | Payload size |
| 16 | ... | AMF payload |

The exact semantics of the control fields are currently unknown but are required for Flash Player to accept the message.
