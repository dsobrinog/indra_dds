#!/bin/bash

export LD_LIBRARY_PATH=/workspaces/fastdds/lib/Fast-DDS/install/lib:$LD_LIBRARY_PATH

EXECUTABLE=${1:-Fast_DDS}

./../build/$EXECUTABLE