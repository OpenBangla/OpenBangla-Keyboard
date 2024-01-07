#!/bin/bash

#VARDOC
#FILE_DIR_OBK (PERSIST) : General path where the scripts will store config, git repos etc etc
#DISTRO_COUNT (TEMP) : Counter to check if user tries to compile for multiple distros without --toolbox flag
#config.bash : As toolbox containers donot inherit any environment variables, PERSISTENT variables are stored in here and loaded into other .sh files using source command
#IM_*_OBK, TOOLBOX_ENABLE_OBK, <DISTRO>*_OBK (PERSIST) : self-explanatory
#cleanup : cleans FILE_DIR_OBK and removes containers created by the script
#help : shows helpinfo

# Guide to add new distros
# 1. add distro switch to the #ARGUMENT HANDLING section of make-pkgs.sh and declare variables appropriately
# 2. add appropriate distro-specific commands to #TOOLBOX USAGE section
# 3. add appropriate distro-specific commands to #TOOLBOX NO USAGE section
# 4. add appropriate make-<DISTRO>.sh file (optional but recommended)
# 5. call make-build.sh with appropriate arguments

#Initial vars
FILE_DIR_OBK="$HOME/.obk-build/"
mkdir "$FILE_DIR_OBK" > /dev/null 2>&1
rm -r "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
touch "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
echo "START_DIR_OBK=$(pwd)" >> "$FILE_DIR_OBK"config.bash
#rm -r "$FILE_DIR_OBK"distro.list > /dev/null 2>&1
#touch "$FILE_DIR_OBK"distro.list > /dev/null 2>&1

#cleanup function
cleanup () {
  echo "Removing obk-toolbox containers..."
  for tbc in $(toolbox list | sed -n 's/.*\(obk-toolbox[^\ ]*\).*/\1/p'); do
    toolbox rm -f "$tbc"
  done
  echo "Cleaning build directory..."
  rm -rf "$FILE_DIR_OBK" > /dev/null 2>&1
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
    echo "  --toolbox       Compile inside of a toolbox"
    echo "  --clean         Clean \"$FILE_DIR_OBK\" directory and remove obk-toolbox-* containers"
    echo "  --help          Display this help message"
    echo " "
    echo "  -> NOTE: User needs to specify the --toolbox flag to compile for multiple distros at the same time."
    echo "           Otherwise, only one flag can be used at a time (e.g. --fedora and --debian can't be used together without --toolbox)"
    echo "  -> NOTE: When compiling inside of a toolbox, an additional <version> can be specified for the distro flags."
    echo "           (e.g. --fedora 37) This will generate packages for a specific version of a distro."
    echo "Example: "
    echo "  $0 --ibus --fcitx --develop --fedora 37 --debian --toolbox"
    echo "  $0 --ibus --develop --debian"
}

#storing vars
DISTRO_COUNT=0

#ARGUMENT HANDLING
for arg in "$@"; do
  case $arg in
    ("--ibus") echo 'IM_IBUS_OBK='"'YES'" >> "$FILE_DIR_OBK"config.bash ;;
    ("--fcitx") echo 'IM_FCITX_OBK='"'YES'" >> "$FILE_DIR_OBK"config.bash ;;
    ("--develop") echo 'BRANCH_OBK='"'develop'" >> "$FILE_DIR_OBK"config.bash ;;
    ("--toolbox") echo 'TOOLBOX_ENABLE_OBK='"'YES'" >> "$FILE_DIR_OBK"config.bash ;;
    ("--fedora")
      echo 'FEDORA_OBK='"'YES'" >> "$FILE_DIR_OBK"config.bash
      echo "FEDORA_VERSION_OBK=$(echo "$@" | sed -n 's/.*--fedora \([0-9]*\).*/\1/p')" >> "$FILE_DIR_OBK"config.bash
      ((DISTRO_COUNT++))
        ;;
    ("--debian")
      echo 'DEBIAN_OBK='"'YES'" >> "$FILE_DIR_OBK"config.bash
      echo "DEBIAN_VERSION_OBK=$(echo "$@" | sed -n 's/.*--debian \([0-9]*\).*/\1/p')" >> "$FILE_DIR_OBK"config.bash
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
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash 2>&1
cat "$FILE_DIR_OBK"config.bash

#TOOLBOX USAGE
if [ "$TOOLBOX_ENABLE_OBK" = 'YES' ]; then
  #create toolbox
  echo "Creating toolbox..."
  toolbox_error () { echo "Something went wrong with the toolbox, Exiting..." && exit 1 ;}
# i attempted at rewriting this part to be distro-agnostic and setup toolboxes from a distro.list file, but the current approach is more modular and more extensible
# e.g. if different distros want to target different/specific toolbox images/repos

# disabled the checking for versions in toolobx sections, the container and pathnames will end up with an extra hyphen - in the end, but it will simplify the toolbox aspect

  #fedora toolbox
  if [ "$FEDORA_OBK" = 'YES' ]; then
    chmod +x make-fedora.sh
    echo "Setting up Fedora toolbox."
#    if [ -z "$FEDORA_VERSION_OBK" ]; then
#    #if the user didnt specified a version
#       ( toolbox create obk-toolbox-fedora -d fedora ) || echo "Failed creating Fedora toolbox" && exit 1
#       ( toolbox run -c obk-toolbox-fedora ./make-fedora.sh ) || echo "Failed entering Fedora toolbox"  && exit 1
#    else
    #if user did
        ( toolbox create obk-toolbox-fedora-"$FEDORA_VERSION_OBK" -d fedora -r "$FEDORA_VERSION_OBK" ) || toolbox_error
        ( toolbox run -c obk-toolbox-fedora-"$FEDORA_VERSION_OBK" ./make-fedora.sh "$FILE_DIR_OBK" ) || toolbox_error
#    fi
  fi

  #debian toolbox
  if [ "$DEBIAN_OBK" = 'YES' ]; then
  chmod +x make-debian.sh
  echo "Setting up Debian toolbox."
#    if [ -z "$DEBIAN_VERSION_OBK" ]; then
#    #if the user didnt specified a version
#        toolbox create obk-toolbox-debian -i quay.io/toolbx-images/debian-toolbox
#        toolbox run -c obk-toolbox-debian ./make-debian.sh
#    else
    #if user did
       ( toolbox create obk-toolbox-debian-"$DEBIAN_VERSION_OBK" -i quay.io/toolbx-images/debian-toolbox:"$DEBIAN_VERSION_OBK" ) || toolbox_error
       ( toolbox run -c obk-toolbox-debian-"$DEBIAN_VERSION_OBK" ./make-debian.sh "$FILE_DIR_OBK" ) || toolbox_error
#    fi
  fi

#TOOLBOX NO USAGE
elif [[ "$DISTRO_COUNT" -ne 1 ]]; then
      echo "Cannot use more than one distro without --toolbox flag."
      echo "Exiting..."
      exit 1
else
  if [ "$FEDORA_OBK" = 'YES' ]; then
    chmod +x make-fedora.sh
    ./make-fedora.sh "$FILE_DIR_OBK"
    exit 1
  fi
  if [ "$DEBIAN_OBK" = 'YES' ]; then
    chmod +x make-debian.sh
    ./make-debian.sh "$FILE_DIR_OBK"

    exit 1
  fi
fi

