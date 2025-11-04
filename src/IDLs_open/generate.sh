#!/bin/bash
set -e

# === Paths ===
OPENDDS_IDL="/workspaces/testdds/lib/OpenDDS/OpenDDS-install/bin/opendds_idl"
IDL_SOURCE_DIR="/workspaces/testdds/src/IDLs_open"
OUTPUT_BASE="./generated"

echo "=== OpenDDS IDL Code Generation ==="

# Add OpenDDS libraries to runtime path (optional, for builds)
export LD_LIBRARY_PATH="/workspaces/testdds/lib/OpenDDS/OpenDDS-install/lib:$LD_LIBRARY_PATH"

mkdir -p "$OUTPUT_BASE"

# Find all .idl files
IDL_FILES=$(find "$IDL_SOURCE_DIR" -maxdepth 1 -name "*.idl" -type f)

if [ -z "$IDL_FILES" ]; then
    echo "No .idl files found in $IDL_SOURCE_DIR"
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

    echo "Generating OpenDDS C++ code for $FILENAME in $OUTPUT_DIR"

    # === OpenDDS IDL Generation ===
    "$OPENDDS_IDL" -o "$OUTPUT_DIR" -DCPS "$IDL_FILE"

    # === Process TypeSupport.idl with tao_idl ===
    TAO_IDL="/workspaces/testdds/lib/OpenDDS/OpenDDS-install/bin/tao_idl"
    TYPE_SUPPORT_IDL="$OUTPUT_DIR/${NAME_NO_EXT}TypeSupport.idl"

    if [ -f "$TYPE_SUPPORT_IDL" ]; then
        echo "Processing $TYPE_SUPPORT_IDL with tao_idl"
        "$TAO_IDL" --idl-version 4 -o "$OUTPUT_DIR" "$TYPE_SUPPORT_IDL"
    else
        echo "Warning: $TYPE_SUPPORT_IDL not found, skipping tao_idl"
    fi


    echo "✓ Completed: $FILENAME"
    echo "Generated files in $OUTPUT_DIR:"
    ls -la "$OUTPUT_DIR"
    echo ""
done

echo "=== OpenDDS IDL generation complete! ==="
