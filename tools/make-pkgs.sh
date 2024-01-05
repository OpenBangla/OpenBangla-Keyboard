#!/bin/bash
rm -r ~/.obk-build/config.bash > /dev/null 2>&1
mkdir ~/.obk-build/ > /dev/null 2>&1
touch ~/.obk-build/config.bash > /dev/null 2>&1

#cleanup function
#help function
#storing args
DISTRO_COUNT=0
for arg in "$@"; do
  case $arg in
    ("--ibus") echo 'IM_IBUS_OBK='YES'' >> ~/.obk-build/config.bash ;;
    ("--fcitx") echo 'IM_FCITX_OBK='YES'' >> ~/.obk-build/config.bash ;;
    ("--develop") echo 'BRANCH_OBK='develop'' >> ~/.obk-build/config.bash ;;
    ("--toolbox") echo 'TOOLBOX_ENABLE_OBK='YES'' >> ~/.obk-build/config.bash ;;
    ("--fedora")
      echo 'FEDORA_OBK='YES'' >> ~/.obk-build/config.bash
      echo "FEDORA_VERSION_OBK="$(echo "$@" | sed -n 's/.*--fedora \([0-9]*\).*/\1/p')"" >> ~/.obk-build/config.bash
      ((DISTRO_COUNT++))
        ;;
    ("--debian")
      echo 'DEBIAN_OBK='YES'' >> ~/.obk-build/config.bash
      echo "DEBIAN_VERSION_OBK="$(echo "$@" | sed -n 's/.*--debian \([0-9]*\).*/\1/p')"" >> ~/.obk-build/config.bash
      ((DISTRO_COUNT++))
         ;;
    (--*)
      echo "Unknown argument: $arg"
      echo "Exiting..."
      exit 1
         ;;
    esac
done

#load vars from file
source ~/.obk-build/config.bash 2>&1
#cat ~/.obk-build/config.bash

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

