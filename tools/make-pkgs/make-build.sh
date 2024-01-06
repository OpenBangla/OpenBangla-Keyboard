#!/bin/bash
source "$BUILD_DIR_OBK"config.bash

#git clone obk, update if exists
if [ -d "$BUILD_DIR_OBK"OpenBangla-Keyboard ]; then
  cd OpenBangla-Keyboard && git pull
else
  ( git clone --recursive https://github.com/OpenBangla/OpenBangla-Keyboard.git && cd OpenBangla-Keyboard ) || ( echo "Error cloning git repo, Exiting..." && rm -rf "$BUILD_DIR_OBK" && exit 1 )
fi
#determine branch
if [ "$BRANCH_OBK" = 'develop' ]; then
  git checkout develop
else
  git checkout master
fi
#update submodules
git submodule update

#compile for ibus and fcitx
if [ -z "$DEBIAN_VERSION_OBK" ]; then
  if [ "$IM_IBUS_OBK" = 'YES' ]; then
    mkdir build-ibus-debian
    cmake -Bbuild-ibus-debian -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-debian
  fi
  if [ "$IM_FCITX_OBK" = 'YES' ]; then
    mkdir build-fcitx-debian
    cmake -Bbuild-fcitx-debian -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-debian
  fi
else
  if [ "$IM_IBUS_OBK" = 'YES' ]; then
    mkdir build-ibus-debian-$DEBIAN_VERSION_OBK
    cmake -Bbuild-ibus-debian-$DEBIAN_VERSION_OBK -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-debian-$DEBIAN_VERSION_OBK
  fi
  if [ "$IM_FCITX_OBK" = 'YES' ]; then
    mkdir build-fcitx-debian-$DEBIAN_VERSION_OBK
    cmake -Bbuild-fcitx-debian-$DEBIAN_VERSION_OBK -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-debian-$DEBIAN_VERSION_OBK
  fi
fi