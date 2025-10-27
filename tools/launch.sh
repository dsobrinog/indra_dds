#!/bin/bash

# Set library path for Fast-DDS
export LD_LIBRARY_PATH=/workspaces/fastdds/lib/Fast-DDS/install/lib:$LD_LIBRARY_PATH

# Default config file (can be overridden with --config)
DEFAULT_CONFIG="../config/default.json"

# Check if user passed --config
CONFIG_ARG="$DEFAULT_CONFIG"
for i in "$@"; do
    if [[ "$i" == --config ]]; then
        CONFIG_ARG=""   # Already provided by user, keep it
        break
    fi
done

# If no --config was passed, append the default
if [[ -z "$CONFIG_ARG" ]]; then
    CMD="./../build/DDS_STRESS_TEST $@"
else
    CMD="./../build/DDS_STRESS_TEST --config $DEFAULT_CONFIG $@"
fi

echo "Launching: $CMD"
eval "$CMD"
