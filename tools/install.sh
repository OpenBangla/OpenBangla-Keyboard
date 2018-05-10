#!/bin/bash

# detect OS
os=$(python -c "exec(\"import platform\\nprint(platform.linux_distribution()[0])\")" )
echo Downloading $os Binary

# get super user access 

# start downloading and install
case $os in

("Ubuntu")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/1.4.0/OpenBangla-Keyboard_1.4.0-ubuntu18.04.deb -O ./OpenBangla.deb
      sudo apt install ./OpenBangla.deb
      sudo rm -f ./OpenBangla.deb
      ;;

("arch")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/1.4.0/openbangla-keyboard-1.4.0-1-x86_64-archlinux.pkg.tar.xz -O /tmp/OpenBangla.pkg.tar.xz
      sudo pacman -U /tmp/OpenBangla.pkg.tar.xz
      sudo rm -f /tmp/OpenBangla.pkg.tar.xz
      ;;
      
("Fedora")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/1.4.0/OpenBangla-Keyboard_1.4.0-fedora27.rpm -O /tmp/OpenBangla.rpm
      sudo dnf install /tmp/OpenBangla.rpm
      sudo rm -f /tmp/OpenBangla.rpm
      ;;

(*)
      echo "Could Not Find your distro"
      ;;

esac
