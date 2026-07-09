## Connection Lifecycle

Establishing a Connection

TFLCSConnect() creates and initializes a TFLCSLocalConnection_t object.

Connection establishment consists of four phases:

1. Validate the supplied listener name and method.
2. Allocate and initialize the connection context.
3. Acquire all required operating system resources.
4. Initialize the Flash LocalConnection shared memory.

The connection is considered usable only after all four phases have completed successfully.

Error Handling

The implementation follows a single-exit cleanup strategy.

Any failure during initialization transfers control to a common cleanup path (errorReturn), which releases all resources through TFLCSDisconnect().

This guarantees that partially initialized connections cannot leak operating system resources.

Connected State

A connection becomes valid only after:

* the shared memory has been attached,
* the semaphore has been opened,
* the Flash shared memory has been initialized, and
* connection->open has been set to 1.