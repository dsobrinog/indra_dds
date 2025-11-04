#!/bin/bash
set -e

# Path to CycloneDDS idlc binary
IDLC="/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-install/bin/idlc"
OUTPUT_BASE="./generated"

echo "=== IDL Code Generation ==="

# Add the Cyclone DDS C++ library path
export LD_LIBRARY_PATH="/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib:$LD_LIBRARY_PATH"

mkdir -p "$OUTPUT_BASE"

# Find all .idl files in the current directory
IDL_FILES=$(find . -maxdepth 1 -name "*.idl" -type f)

if [ -z "$IDL_FILES" ]; then
    echo "No .idl files found"
    exit 0
fi

echo "Found IDL files:"
echo "$IDL_FILES"
echo ""

for IDL_FILE in $IDL_FILES; do
    FILENAME=$(basename "$IDL_FILE")
    NAME_NO_EXT="${FILENAME%.idl}"
    OUTPUT_DIR="$OUTPUT_BASE/$NAME_NO_EXT"

    if [ -d "$OUTPUT_DIR" ]; then
        echo "Removing old generated code for $NAME_NO_EXT"
        rm -rf "$OUTPUT_DIR"
    fi
    mkdir -p "$OUTPUT_DIR"

    echo "Generating C++ code for $FILENAME in $OUTPUT_DIR"

    # Run the idlc compiler with C++ backend
    "$IDLC" -l cxx -d "$OUTPUT_DIR" "$IDL_FILE"

    echo "✓ Completed: $FILENAME"
    echo "Generated files in $OUTPUT_DIR:"
    ls -la "$OUTPUT_DIR"
    echo ""
done
