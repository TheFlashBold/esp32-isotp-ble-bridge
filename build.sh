#!/bin/bash

set -e

cd "$(dirname "$0")"

source ~/esp/esp-idf/export.sh

if [ -f build/.bin_timestamp ] && [ ! -f build/isotp_ble_bridge.bin ]; then
    echo "Removing stale build/.bin_timestamp so the app binary is regenerated"
    rm build/.bin_timestamp
fi

~/esp/esp-idf/tools/idf.py build
