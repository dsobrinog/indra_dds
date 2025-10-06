#!/bin/bash
# Script to generate Fast DDS code for all IDL files in current directory
# Each IDL will have its own subfolder in ./generated, replacing previous content

# Exit on any error
set -e

# Path to Fast DDS generator
FASTDDSGEN="../../lib/Fast-DDS/fastddsgen/scripts/fastddsgen"

# Check if the generator exists
if [ ! -x "$FASTDDSGEN" ]; then
    echo "Error: fastddsgen not found or not executable at $FASTDDSGEN"
    exit 1
fi

# Base output folder
OUTPUT_BASE="./generated"
mkdir -p "$OUTPUT_BASE"

# Find all .idl files in current directory
IDL_FILES=$(find . -maxdepth 1 -name "*.idl" -type f)

# Check if any IDL files were found
if [ -z "$IDL_FILES" ]; then
    echo "No .idl files found in current directory"
    exit 0
fi

echo "Found IDL files:"
echo "$IDL_FILES"
echo ""

# Process each IDL file
for IDL_FILE in $IDL_FILES; do
    FILENAME=$(basename "$IDL_FILE")
    NAME_NO_EXT="${FILENAME%.idl}"

    # Create or replace folder for this specific IDL
    OUTPUT_DIR="$OUTPUT_BASE/$NAME_NO_EXT"
    if [ -d "$OUTPUT_DIR" ]; then
        echo "Removing old generated code for $NAME_NO_EXT"
        rm -rf "$OUTPUT_DIR"
    fi
    mkdir -p "$OUTPUT_DIR"

    echo "Generating code for $FILENAME in $OUTPUT_DIR"

    # Run fastddsgen for this IDL file
    "$FASTDDSGEN" -d "$OUTPUT_DIR" "$IDL_FILE"

    echo "✓ Completed: $FILENAME"
    echo ""
done

echo "All IDL files processed successfully!"
echo "Generated code can be found in '$OUTPUT_BASE'"
