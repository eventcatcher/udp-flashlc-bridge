Shared Memory Discovery

Overview

Before data can be delivered to a Flash application, the bridge must locate the shared memory segment used by Flash’s LocalConnection implementation.

The discovery mechanism differs between Windows and POSIX systems.

Windows

On Windows, Flash uses a named shared memory object.

The bridge simply opens the predefined shared memory name.

No discovery step is required.

macOS / POSIX

On macOS, Flash uses a System V shared memory segment identified by a key_t.

The bridge first attempts to locate an existing segment using a list of known keys that were identified through reverse engineering of Flash Player.

If none of the known keys exists, the bridge falls back to the first known key and creates the shared memory segment itself.

This allows the bridge to be started before the Flash application.

Notes

The source code contains an alternative implementation that scans all System V shared memory segments using shmctl(). This implementation is disabled because probing a small set of known keys is considerably faster and proved sufficient in practice.