#!/bin/bash
echo "Generating Debian packages..."
source "$BUILD_DIR_OBK"config.bash > /dev/null 2>&1
cd "$BUILD_DIR_OBK"

#update the system and install builddeps
sudo apt update -y && sudo apt upgrade -y
sudo apt-get install -y build-essential file rustc cargo cmake meson ninja-build libibus-1.0-dev libfcitx5core-dev fcitx5 qtbase5-dev qtbase5-dev-tools libzstd-dev git

#call buildscript
chmod +x make-build.sh && ./make-build.sh