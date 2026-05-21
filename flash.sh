#!/bin/bash

set -e

source ~/esp/esp-idf/export.sh
~/esp/esp-idf/tools/idf.py flash -p /dev/cu.usbserial-0001
