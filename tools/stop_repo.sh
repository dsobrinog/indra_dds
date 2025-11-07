#!/bin/bash
set -e

IOR_FILE="/workspaces/testdds/tools/repo.ior"

echo "[OpenDDS] Stopping DCPSInfoRepo..."

# Find and stop any running DCPSInfoRepo process
if pgrep -f DCPSInfoRepo >/dev/null; then
    pkill -f DCPSInfoRepo
    echo "[OpenDDS] DCPSInfoRepo stopped."
else
    echo "[OpenDDS] No running DCPSInfoRepo found."
fi

# Optionally clean up old repo file
if [[ -f "$IOR_FILE" ]]; then
    rm -f "$IOR_FILE"
    echo "[OpenDDS] Removed old repo.ior file."
fi
