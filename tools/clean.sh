#!/bin/bash
BUILD_DIR="../build"
if [ -d "$BUILD_DIR" ]; then
    echo "Removing $BUILD_DIR directory..."
    rm -rf "$BUILD_DIR"
    echo "$BUILD_DIR directory removed."
else
    echo "$BUILD_DIR directory does not exist. No action taken."
fi
