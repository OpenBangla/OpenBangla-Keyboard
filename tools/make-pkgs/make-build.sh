#!/bin/bash

# If insufficent args provided, exit
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Insufficent arguments for this script, Exiting..."
    exit 1
fi

FILE_DIR_OBK="$2"
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash
cd "$FILE_DIR_OBK" || { echo "Error entering FILE_DIR_OBK, Exiting..." && exit 1 ;}

# This script takes the first argument as the "distro string", and that is used to name builddirs and determine the distro/version etc
# The second argument points to FILE_DIR_OBK to load vars from

#git clone obk, update if exists
if [ -d "$FILE_DIR_OBK"OpenBangla-Keyboard ]; then
  cd OpenBangla-Keyboard && git pull
else
  ( git clone --recursive https://github.com/OpenBangla/OpenBangla-Keyboard.git && cd OpenBangla-Keyboard ) || { echo "Error cloning git repo, Exiting..." && rm -rf "$FILE_DIR_OBK" && exit 1 ;}
fi
#determine branch
if [ "$BRANCH_OBK" = 'develop' ]; then
  git checkout develop
else
  git checkout master
fi
#update submodules
git submodule update

#determine packaging format
PACKAGE_FORMAT=""
case "$1" in
    (*fedora*) PACKAGE_FORMAT="RPM" ;;
    (*debian*) PACKAGE_FORMAT="DEB" ;;
esac

#compile for ibus and fcitx
  if [ "$IM_IBUS_OBK" = 'YES' ]; then
    mkdir build-ibus-"$1"
    cmake -Bbuild-ibus-"$1" -GNinja -DCPACK_GENERATOR="$PACKAGE_FORMAT" -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-"$1"
  fi
  if [ "$IM_FCITX_OBK" = 'YES' ]; then
    mkdir build-fcitx-"$1"
    cmake -Bbuild-fcitx-"$1" -GNinja -DCPACK_GENERATOR="$PACKAGE_FORMAT" -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-"$1"
  fi