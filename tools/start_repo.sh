
#!/bin/bash
set -e

OPENDDS_BIN="/workspaces/testdds/lib/OpenDDS/OpenDDS-install/bin"
IOR_FILE="/workspaces/testdds/tools/repo.ior"

# Kill any old repo processes
pkill -f DCPSInfoRepo 2>/dev/null || true

# Remove previous repo file
rm -f "$IOR_FILE"

echo "[OpenDDS] Starting DCPSInfoRepo..."
"$OPENDDS_BIN/DCPSInfoRepo" -o "$IOR_FILE" &

# Wait until IOR file is created
for i in {1..10}; do
    if [[ -f "$IOR_FILE" ]]; then
        echo "[OpenDDS] repo.ior ready at $IOR_FILE"
        echo "export DCPSInfoRepo=file:///workspaces/testdds/tools/repo.ior" >> ~/.bashrc
        # echo "[OpenDDS] Export this in your terminal before launching apps:"
        # echo "    export DCPSInfoRepo=file://$IOR_FILE"
        exit 0
    fi
    sleep 0.5

done

echo "[OpenDDS] ERROR: repo.ior not created after 5 seconds."
exit 1
