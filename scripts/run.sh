#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

"$SCRIPT_DIR/../build/udp-flashlc-bridge" \
    -l 2

echo
read -n 1 -s -r -p "Press any key to close..."
