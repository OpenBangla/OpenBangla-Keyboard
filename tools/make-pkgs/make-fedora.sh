#!/bin/bash

# If insufficent args provided, exit
if [ -z "$1" ]; then
    echo "Insufficent arguments for this script, Exiting..."
    exit 1
fi

FILE_DIR_OBK="$1"
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
echo "Generating Fedora packages..."
#cd "$FILE_DIR_OBK" || exit 1

#update the system and install builddeps
sudo dnf -y update
sudo dnf -y install @buildsys-build rust cargo cmake qt5-qtdeclarative-devel libzstd-devel fcitx5 fcitx5-devel ibus ibus-devel ninja-build curl meson git

#call buildscript
chmod +x make-build.sh && ./make-build.sh fedora-"$FEDORA_VERSION_OBK" "$FILE_DIR_OBK"