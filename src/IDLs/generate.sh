#!/bin/bash
# Simple script to generate Fast DDS code for inse.idl

# Exit on any error
set -e

# Path to Fast DDS generator
FASTDDSGEN="../../lib/Fast-DDS/fastddsgen/scripts/fastddsgen"

# Check if the generator exists
if [ ! -x "$FASTDDSGEN" ]; then
    echo "Error: fastddsgen not found or not executable at $FASTDDSGEN"
    exit 1
fi

# Create output folder if it does not exist
OUTPUT_DIR="./generated"
mkdir -p "$OUTPUT_DIR"

# Run fastddsgen
"$FASTDDSGEN" -example CMake -d "$OUTPUT_DIR" inse.idl

echo "Fast DDS code generated in $OUTPUT_DIR"
