#!/bin/bash
echo "Generating Fedora packages..."
source ~/.obk-build/config.bash > /dev/null 2>&1
cd ~/.obk-build/

#update the system and install builddeps
sudo dnf -y update
sudo dnf -y install @buildsys-build rust cargo cmake qt5-qtdeclarative-devel libzstd-devel fcitx5 fcitx5-devel ibus ibus-devel ninja-build curl meson git

#git clone obk, update if exists
if [ -d ~/.obk-build/OpenBangla-Keyboard ]; then
  cd OpenBangla-Keyboard && git pull
else
  git clone --recursive https://github.com/OpenBangla/OpenBangla-Keyboard.git && cd OpenBangla-Keyboard
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
if [ -z "$FEDORA_VERSION_OBK" ]; then
  if [ "$IM_IBUS_OBK" = 'YES' ]; then
    mkdir build-ibus-fedora
    cmake -Bbuild-ibus-fedora -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-fedora
  fi
  if [ "$IM_FCITX_OBK" = 'YES' ]; then
    mkdir build-fcitx-fedora
    cmake -Bbuild-fcitx-fedora -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-fedora
  fi
else
  if [ "$IM_IBUS_OBK" = 'YES' ]; then
    mkdir build-ibus-fedora-$FEDORA_VERSION_OBK
    cmake -Bbuild-ibus-fedora-$FEDORA_VERSION_OBK -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-fedora-$FEDORA_VERSION_OBK
  fi
  if [ "$IM_FCITX_OBK" = 'YES' ]; then
    mkdir build-fcitx-fedora-$FEDORA_VERSION_OBK
    cmake -Bbuild-fcitx-fedora-$FEDORA_VERSION_OBK -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-fedora-$FEDORA_VERSION_OBK
  fi
fi