#!/bin/bash
echo "Generating Fedora packages..."
source "$BUILD_DIR_OBK"config.bash > /dev/null 2>&1
cd "$BUILD_DIR_OBK"

#update the system and install builddeps
sudo dnf -y update
sudo dnf -y install @buildsys-build rust cargo cmake qt5-qtdeclarative-devel libzstd-devel fcitx5 fcitx5-devel ibus ibus-devel ninja-build curl meson git

#call buildscript
chmod +x make-build.sh && ./make-build.sh