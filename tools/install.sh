#!/bin/bash


os=$(python -c "exec(\"import platform\\nprint(platform.linux_distribution()[0])\")" )
echo $os


case $os in

("Ubuntu")
      wget https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/1.4.0/OpenBangla-Keyboard_1.4.0-ubuntu18.04.deb -O /tmp/OpenBangla.deb
      sudo dpkg -i /tmp/OpenBangla.deb
      ;;

("arch")
      wget https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/1.4.0/openbangla-keyboard-1.4.0-1-x86_64-archlinux.pkg.tar.xz -O /tmp/OpenBangla.pkg.tar.xz
      sudo pacman -U /tmp/OpenBangla.pkg.tar.xz
      ;;

(*)
      echo "Could Not Find your distro"
      ;;

esac
