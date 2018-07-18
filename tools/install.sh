#!/bin/bash
##  Note for Maintainter: Always update this whenever a new version is released. Also remember to update download URLs if needed
APP_VERSION=1.4.0
URL_STUB="https://github.com/OpenBangla/OpenBangla-Keyboard/releases/download/${APP_VERSION}"

cat <<"EOF"
|-------------------------------------------------------|
|-----Welcome to the OpenBangla Keyboard installer------|
|-------------------------------------------------------|

This helper script will install/update OpenBangla Keyboard in your system.
You might need to type your password for the installation to proceed.

After the installation completes successfully, you would need to configure
your desktop environment for OpenBangla Keyboard to work.
Visit https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Configuring-Environment
for instructions on how to configure your desktop environment.
--------------------------------------------------------

EOF

# detect distro
function get_distro {
  local distro_name distro_data distro_str
  # try to get distro data from *-release files
  distro_data=$(cat /etc/*-release)
  while read -r line; do
      if [[ $line =~ ^(NAME|DISTRIB_ID)=(.+)$ ]]; then
      distro_str=${BASH_REMATCH[2]}
      if echo "$distro_str" | grep -qP '(?i).*ubuntu.*'; then
        distro_name='ubuntu'
      elif echo "$distro_str" | grep -qP '(?i).*fedora.*'; then
        distro_name='fedora'
      elif echo "$distro_str" | grep -qP '(?i)^[''"]?(arch|manjaro|antergos|chakra|magpie|parabola|anarchy).*'; then
        distro_name='arch'
      fi
      [ $distro_name ] && break
    fi
  done <<< "$distro_data"
  # plain arch installation doesn't guarantee /etc/os-release
  # but the filesystem pkg installs a blank /etc/arch-release
  if [ -z "$distro_name" ]; then
    if [ -e /etc/arch-release ]; then
      distro_name='arch'
    fi
  fi
  # TODO if os not detected, try package-manager detection (not 100% reliable)
  echo "$distro_name"
}

DISTRO_NAME=$(get_distro)
echo "Downloading installation package for $DISTRO_NAME ..."
# start downloading and install
case $DISTRO_NAME in
("ubuntu")
  # VERSION_ID=$(lsb_release -r | cut -f 2)  #  lsb_release may be replaced as well with shell script
  # ubuntu is supposed to ensure a /etc/os-release file by default
  # VERSION_ID (and some more vars) are sourced
  # shellcheck disable=SC1091
  source /etc/os-release
  if [[ $VERSION_ID = "18.04" || $VERSION_ID = "16.04" ]]; then
    wget -q --show-progress "${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-ubuntu${VERSION_ID}.deb" -O "$HOME/OpenBangla.deb"
    sudo apt install "$HOME/OpenBangla.deb"
    mv -f "$HOME/OpenBangla.deb" /tmp/
  else
    echo "Ubuntu version not supported ($VERSION_ID)."
  fi
  ;;
("arch")
  # this url should proably be normalized soon
  wget -q --show-progress "${URL_STUB}/openbangla-keyboard-${APP_VERSION}-1-x86_64-archlinux.pkg.tar.xz" -O /tmp/OpenBangla.pkg.tar.xz
  sudo pacman -U /tmp/OpenBangla.pkg.tar.xz
  ;;
("fedora")
  # fedora version check ?
  wget -q --show-progress "${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-fedora27.rpm" -O /tmp/OpenBangla.rpm
  sudo dnf install /tmp/OpenBangla.rpm
  ;;
(*)
      echo
      echo "This script was not able to identify your OS distribution! Please report a bug at https://github.com/OpenBangla/OpenBangla-Keyboard/issues"
      echo
      echo "Please visit https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard for Distrowise/Distro-specific Install Instructions."
      echo
      
#       echo "Do you want to see the instructions for installing OpenBangla Keyboard in your system?"
#       select yn in "Yes" "No"; do
      
#       case $yn in
            
#         (Yes)
#           URL="https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard"
#           [[ -x $BROWSER ]] && exec "$BROWSER" "$URL"
#           path=$(command -v xdg-open || command -v gnome-open) && exec "$path" "$URL"
#           echo "Can't find browser"
#           break
#           ;; 
#         (No)
#           exit
#           ;; 
#         esac
      
#       done
#     ;; 
echo -n "Do you want to see the instructions? (Yes/No) "
read -r $answer
case $answer in
  [yY]*)
    URL="https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard"
    [[ -x $BROWSER ]] && exec "$BROWSER" "$URL"
    path=$(command -v xdg-open || command -v gnome-open) && exec "$path" "$URL"
    echo "Can't find browser"
    break
    ;;
  [nN]*)
    exit
    ;;
esac
