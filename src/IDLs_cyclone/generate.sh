#!/bin/bash
set -e

IDLC="/opt/cyclonedds/install/bin/idlc"
OUTPUT_BASE="./generated"

echo "=== IDL Code Generation ==="
echo "Using generator: cxx (instead of c++)"

# Set library paths for runtime
export LD_LIBRARY_PATH="/opt/cyclonedds/install/lib:/opt/cyclonedds/install/cxx/lib:$LD_LIBRARY_PATH"

# Set path for idlc generator plugins
export IDL_GENERATOR_PATH="/opt/cyclonedds/install/cxx/lib"

mkdir -p "$OUTPUT_BASE"

# Find all .idl files
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

    echo "Generating code for $FILENAME in $OUTPUT_DIR"

    # Use the correct generator name: cxx instead of c++
    LD_LIBRARY_PATH="/opt/cyclonedds/install/lib:/opt/cyclonedds/install/cxx/lib" "$IDLC" -l c -o "$OUTPUT_DIR" "$IDL_FILE"

    echo "✓ Completed: $FILENAME"
    echo "Generated files in $OUTPUT_DIR:"
    ls -la "$OUTPUT_DIR"
    echo ""
done

echo "All IDL files processed successfully!"
echo "Generated code can be found in '$OUTPUT_BASE'"