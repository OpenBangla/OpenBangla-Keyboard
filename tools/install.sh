#!/bin/bash
## MUST: Always update this whenever a new version is released.
VERSION=1.4.0

echo "|-------------------------------------------------------|"
echo "|-----Welcome to the OpenBangla Keyboard installer------|"
echo "|-------------------------------------------------------|"
echo
echo "This helper script will install/update OpenBangla Keyboard in your system."
echo "You might need to type your password for the installation to proceed."
echo
echo "After the installation completes successfully, you would need to configure your desktop environment for OpenBangla Keyboard to work."
echo "Visit https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Configuring-Environment for instructions to configure your desktop environment."
echo

# detect OS
os=$(python -c "exec(\"import platform\\nprint(platform.linux_distribution()[0])\")" )
echo "Downloading installation package for $os"

# start downloading and install
case $os in

("Ubuntu")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/$VERSION/OpenBangla-Keyboard_$VERSION-ubuntu18.04.deb -O ./OpenBangla.deb
      sudo apt install ./OpenBangla.deb
      sudo rm -f ./OpenBangla.deb
      ;;

("arch")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/$VERSION/OpenBangla-Keyboard_$VERSION-archlinux.pkg.tar.xz -O /tmp/OpenBangla.pkg.tar.xz
      sudo pacman -U /tmp/OpenBangla.pkg.tar.xz
      ;;
      
("Fedora")
      wget -q --show-progress https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/$VERSION/OpenBangla-Keyboard_$VERSION-fedora27.rpm -O /tmp/OpenBangla.rpm
      sudo dnf install /tmp/OpenBangla.rpm
      sudo rm -f /tmp/OpenBangla.rpm
      ;;

(*)
      echo
      echo "This script was not able to identify your OS distribution! Please report a bug at https://github.com/OpenBangla/OpenBangla-Keyboard/issues"
      echo
      echo "Please visit https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard for Distrowise/Distro-specific Install Instructions."
      echo
      ;;

esac
