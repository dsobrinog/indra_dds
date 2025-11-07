#!/bin/bash
set -e

# === Paths ===
OPENDDS_ROOT="/workspaces/testdds/lib/OpenDDS/OpenDDS-install"
OPENDDS_IDL="$OPENDDS_ROOT/bin/opendds_idl"
TAO_IDL="$OPENDDS_ROOT/bin/tao_idl"
IDL_SOURCE_DIR="/workspaces/testdds/src/IDLs_open"
GENERATED_DIR="./generated"
OPENDDS_SHARE="$OPENDDS_ROOT/share"

echo "=== OpenDDS IDL Code Generation ==="

# Add OpenDDS libraries to runtime path (optional, for builds)
export LD_LIBRARY_PATH="$OPENDDS_ROOT/lib:$LD_LIBRARY_PATH"

mkdir -p "$GENERATED_DIR"

# Find all .idl files
IDL_FILES=$(find "$IDL_SOURCE_DIR" -maxdepth 1 -name "*.idl" -type f)

if [ -z "$IDL_FILES" ]; then
    echo "No .idl files found in $IDL_SOURCE_DIR"
    exit 0
fi

GENERATED_SOURCES=()

for IDL_FILE in $IDL_FILES; do
    BASE_NAME=$(basename "$IDL_FILE" .idl)
    GEN_DIR="$GENERATED_DIR/$BASE_NAME"

    if [ -d "$GEN_DIR" ]; then
        echo "Removing old generated code for $BASE_NAME"
        rm -rf "$GEN_DIR"
    fi
    mkdir -p "$GEN_DIR"

    echo "Generating OpenDDS C++ code for $BASE_NAME.idl in $GEN_DIR"

    # === Step 1: opendds_idl ===
    "$OPENDDS_IDL" -Lc++11 -o "$GEN_DIR" "$IDL_FILE"

    # === Step 2: tao_idl ===
    TYPE_SUPPORT_IDL="$GEN_DIR/${BASE_NAME}TypeSupport.idl"
    if [ -f "$TYPE_SUPPORT_IDL" ]; then
        echo "Processing $TYPE_SUPPORT_IDL with tao_idl"
        "$TAO_IDL" \
            --idl-version 4 \
            -I"$OPENDDS_SHARE" \
            -I"$OPENDDS_ROOT/include" \
            -I"/workspaces/testdds/lib/OpenDDS/OpenDDS/dds/idl" \
            -I"$IDL_SOURCE_DIR" \
            -I"$OPENDDS_ROOT/include/dds" \
            -o "$GEN_DIR" \
            "$TYPE_SUPPORT_IDL"
    else
        echo "Warning: $TYPE_SUPPORT_IDL not found, skipping tao_idl"
    fi

    # === Track generated source files ===
    GENERATED_SOURCES+=(
        "$GEN_DIR/${BASE_NAME}TypeSupportImpl.cpp"
        "$GEN_DIR/${BASE_NAME}TypeSupportC.cpp"
        "$GEN_DIR/${BASE_NAME}TypeSupportS.cpp"
    )

    echo "✓ Completed: $BASE_NAME.idl"
    echo "Generated files in $GEN_DIR:"
    ls -la "$GEN_DIR"
    echo ""
done

echo "All generated sources:"
for src in "${GENERATED_SOURCES[@]}"; do
    echo "$src"
done

echo "=== OpenDDS IDL generation complete! ==="
