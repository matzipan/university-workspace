#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /usr/userfs/a/az579/Workspace/EMBS/FPGA/P6/divcore/divcore/solution1/.autopilot/db/a.g.bc ${1+"$@"}