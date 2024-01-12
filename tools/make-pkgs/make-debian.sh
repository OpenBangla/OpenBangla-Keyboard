#!/bin/bash
echo "Generating Debian packages..."

# If insufficent args provided, exit
if [[ -z "$1" ]]; then
    echo "Insufficent arguments for this script, Exiting..."
    exit 1
fi

FILE_DIR_OBK="$1"
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
#DEBUG
log_debug "$0"

#update the system and install builddeps
{
    sudo apt update -y && sudo apt upgrade -y
    sudo apt-get install -y build-essential file rustc cargo cmake meson ninja-build libibus-1.0-dev libfcitx5core-dev fcitx5 qtbase5-dev qtbase5-dev-tools libzstd-dev git
} || { echo "Error: could not install packages. Exiting..." && exit 1 ;}

#call buildscript
chmod +x make-build.sh && ./make-build.sh "$FILE_DIR_OBK" debian-"$DEBIAN_VERSION_OBK"