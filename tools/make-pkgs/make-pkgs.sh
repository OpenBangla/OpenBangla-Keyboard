#!/bin/bash
BUILD_DIR_OBK="$HOME/.obk-build/"
rm -r "$BUILD_DIR_OBK"{config.bash,distro.list} > /dev/null 2>&1
mkdir "$BUILD_DIR_OBK" > /dev/null 2>&1
touch "$BUILD_DIR_OBK"config.bash > /dev/null 2>&1
touch "$BUILD_DIR_OBK"distro.list > /dev/null 2>&1

#cleanup function
cleanup () {
  echo "Removing obk-toolbox containers..."
  for tbc in $(toolbox list | sed -n 's/.*\(obk-toolbox[^\ ]*\).*/\1/p'); do
    toolbox rm -f "$tbc"
  done
  echo "Cleaning build directory..."
  rm -rf "$BUILD_DIR_OBK" > /dev/null 2>&1
}
#help function
help () {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --ibus          Compile IBus version"
    echo "  --fcitx         Compile Fcitx version (Doesn't work for OBK2 and earlier)"
    echo "  --develop       Compile from the develop branch"
    echo "  --fedora        Compile for Fedora"
    echo "  --debian        Compile for Debian"
    echo "    NOTE: User needs to specify the --toolbox flag to compile for multiple distros at the same time."
    echo "          Otherwise, only one flag can be used at a time (e.g. --fedora and --debian can't be used together without --toolbox)"
    echo "  --toolbox       Compile inside of a toolbox"
    echo "    NOTE: When compiling inside of a toolbox, an additional <version> can be specified for the distro flags."
    echo "          (e.g. --fedora 37) This will generate packages for a specific version of a distro."
    echo "  --clean         Clean \"$BUILD_DIR_OBK\" directory and remove obk-toolbox-* containers"
    echo "  --help          Display this help message"
    echo "Example: "
    echo "  $0 --ibus --fcitx --develop --fedora 37 --debian --toolbox"
    echo "  $0 --ibus --develop --debian"
}

#storing args
DISTRO_COUNT=0
for arg in "$@"; do
  case $arg in
    ("--ibus") echo 'IM_IBUS_OBK='"'YES'" >> "$BUILD_DIR_OBK"config.bash ;;
    ("--fcitx") echo 'IM_FCITX_OBK='"'YES'" >> "$BUILD_DIR_OBK"config.bash ;;
    ("--develop") echo 'BRANCH_OBK='"'develop'" >> "$BUILD_DIR_OBK"config.bash ;;
    ("--toolbox") echo 'TOOLBOX_ENABLE_OBK='"'YES'" >> "$BUILD_DIR_OBK"config.bash ;;
    ("--fedora")
      echo 'FEDORA_OBK='"'YES'" >> "$BUILD_DIR_OBK"config.bash
      echo "FEDORA_VERSION_OBK=$(echo "$@" | sed -n 's/.*--fedora \([0-9]*\).*/\1/p')" >> "$BUILD_DIR_OBK"config.bash
      ((DISTRO_COUNT++))
        ;;
    ("--debian")
      echo 'DEBIAN_OBK='"'YES'" >> "$BUILD_DIR_OBK"config.bash
      echo "DEBIAN_VERSION_OBK=$(echo "$@" | sed -n 's/.*--debian \([0-9]*\).*/\1/p')" >> "$BUILD_DIR_OBK"config.bash
      ((DISTRO_COUNT++))
        ;;
    ("--clean") 
      cleanup
      exit 1
        ;;
    ("--help")
      help
      exit 1
        ;;
    (--*)
      echo "Unknown argument: $arg"
      help
      echo "Exiting..."
      exit 1 ;;
    esac
done

#load vars from file
source "$BUILD_DIR_OBK"config.bash 2>&1
cat "$BUILD_DIR_OBK"config.bash

#if user wants to use a toolbox
if [ "$TOOLBOX_ENABLE_OBK" = 'YES' ]; then
  #create toolbox
  echo "Creating toolbox..."

  #fedora toolbox
  if [ "$FEDORA_OBK" = 'YES' ]; then
    chmod +x make-fedora.sh
    if [ -z "$FEDORA_VERSION_OBK" ]; then
    #if the user didnt specified a version
        toolbox create obk-toolbox-fedora -d fedora
        toolbox run -c obk-toolbox-fedora ./make-fedora.sh
        exit 1
    else
    #if user did
        toolbox create obk-toolbox-fedora-"$FEDORA_VERSION_OBK" -d fedora -r "$FEDORA_VERSION_OBK"
        toolbox run -c obk-toolbox-fedora-"$FEDORA_VERSION_OBK" ./make-fedora.sh
        exit 1
    fi
  fi

  #debian toolbox
  if [ "$DEBIAN_OBK" = 'YES' ]; then
    chmod +x make-debian.sh
    if [ -z "$DEBIAN_VERSION_OBK" ]; then
    #if the user didnt specified a version
        toolbox create obk-toolbox-debian -i quay.io/toolbx-images/debian-toolbox
        toolbox run -c obk-toolbox-debian ./make-debian.sh
        exit 1
    else
    #if user did
        toolbox create obk-toolbox-debian-"$DEBIAN_VERSION_OBK" -i quay.io/toolbx-images/debian-toolbox:"$DEBIAN_VERSION_OBK"
        toolbox run -c obk-toolbox-debian-"$DEBIAN_VERSION_OBK" ./make-debian.sh
        exit 1
    fi
  fi

#if user doesn't want ot use a toolbox
elif [[ "$DISTRO_COUNT" -ne 1 ]]; then
      echo "Cannot use more than one distro without --toolbox flag."
      echo "Exiting..."
      exit 1
else
  if [ "$FEDORA_OBK" = 'YES' ]; then
    chmod +x make-fedora.sh
    ./make-fedora.sh
    exit 1
  fi
  if [ "$DEBIAN_OBK" = 'YES' ]; then
    chmod +x make-debian.sh
    ./make-debian.sh
    exit 1
  fi
fi

