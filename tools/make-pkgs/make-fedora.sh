#!/bin/bash
echo "Generating Fedora packages..."

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
    sudo dnf -y update
    sudo dnf -y install @buildsys-build rust cargo cmake qt5-qtdeclarative-devel libzstd-devel fcitx5 fcitx5-devel ibus ibus-devel ninja-build curl meson git
} || { echo "Error: could not install packages. Exiting..." && exit 1 ;}

#call buildscript
chmod +x make-build.sh && ./make-build.sh "$FILE_DIR_OBK" fedora-"$FEDORA_VERSION_OBK"