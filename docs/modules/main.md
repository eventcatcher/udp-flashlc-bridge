Application Entry Point

Responsibilities

• Parse command line arguments

• Create the UDP listener

• Create the Flash LocalConnection bridge

• Register the UDP callback

• Enter the main receive loop

The application itself contains almost no protocol logic.
Networking is implemented in UDPListener.
Flash communication is implemented in TFFlashLCSHMEM.
