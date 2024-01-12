#!/bin/bash

# If insufficent args provided, exit
if [[ -z "$1" ]]; then
    echo "Insufficent arguments for this script, Exiting..."
    exit 1
fi

FILE_DIR_OBK="$1"
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
echo "Generating Debian packages..."
#cd "$FILE_DIR_OBK" || exit 1

#update the system and install builddeps
sudo apt update -y && sudo apt upgrade -y
sudo apt-get install -y build-essential file rustc cargo cmake meson ninja-build libibus-1.0-dev libfcitx5core-dev fcitx5 qtbase5-dev qtbase5-dev-tools libzstd-dev git

#call buildscript
chmod +x make-build.sh && ./make-build.sh debian-"$DEBIAN_VERSION_OBK" "$FILE_DIR_OBK"