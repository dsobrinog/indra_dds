#!/bin/bash

export LD_LIBRARY_PATH=/workspaces/fastdds/lib/Fast-DDS/install/lib:$LD_LIBRARY_PATH

./../build/Fast_DDS "$@"
