#!/bin/bash
echo "Generating Fedora packages..."
source ~/.obk-build/config.bash
cd ~/.obk-build/

#update the system and install builddeps
sudo dnf -y update
sudo dnf -y install @buildsys-build rust cargo cmake qt5-qtdeclarative-devel libzstd-devel fcitx5 fcitx5-devel ibus ibus-devel ninja-build curl meson

#git clone obk, update if exists
if [ -d "~/.obk-build/OpenBangla-Keyboard/" ]; then
  cd OpenBangla-Keyboard
else
  git clone --recursive https://github.com/OpenBangla/OpenBangla-Keyboard.git && cd OpenBangla-Keyboard
fi
#determine branch
if [ $BRANCH_OBK = 'develop' ]; then
  git checkout develop
else
  git checkout master
fi
#update submodules and repo
git pull --force
git submodule update

#compile for ibus
if [ $IM_IBUS_OBK = 'YES' ]; then
  mkdir build-ibus-fedora
  cmake -Bbuild-ibus-fedora -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON && ninja package -C build-ibus-fedora
fi
if [ $IM_FCITX_OBK = 'YES' ]; then
  mkdir build-fcitx-fedora
  cmake -Bbuild-fcitx-fedora -GNinja -DCPACK_GENERATOR=RPM -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON && ninja package -C build-fcitx-fedora
fi

cp ./build-*-fedora/*.rpm ~/
