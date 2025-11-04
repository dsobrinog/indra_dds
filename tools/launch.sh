#!/bin/bash
export LD_LIBRARY_PATH=/workspaces/testdds/lib/Fast-DDS/install/lib:$LD_LIBRARY_PATH

# Absolute path to binary
BINARY="/workspaces/testdds/build/DDS_STRESS_TEST"

# Absolute default config
DEFAULT_CONFIG="/workspaces/testdds/config/default.json"

# Check if --config is already passed
HAS_CONFIG=false
for arg in "$@"; do
    if [[ "$arg" == --config ]]; then
        HAS_CONFIG=true
        break
    fi
done

# Build command array
CMD=("$BINARY")
if [ "$HAS_CONFIG" = false ]; then
    CMD+=(--config "$DEFAULT_CONFIG")
fi

if [ -n "$LOG_FILE" ]; then
    # Replace ${HOSTNAME} with the container's hostname
    LOG_FILE="${LOG_FILE//\$\{HOSTNAME\}/$HOSTNAME}"
    CMD+=(--logFile "$LOG_FILE")
fi
# Append remaining arguments
CMD+=("$@")

# Debug output
echo "Launching: ${CMD[@]}"

# Execute
"${CMD[@]}"
