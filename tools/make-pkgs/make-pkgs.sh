#!/bin/bash

# Variable documentation -> see function.bash
# Guide to add new distros
# 1. add distro switch to parse_args function of function.bash of declare variables <DISTRO>_OBK and <DISTRO_VERSION>_OBK appropriately
# 2. add appropriate make-<DISTRO>.sh file (optional but recommended)
# 3. add appropriate distro-specific commands to #TOOLBOX USAGE section (use #fedora toolbox and #debian toolbox as templates)
# 4. add appropriate distro-specific commands to #TOOLBOX NO USAGE section
# 5. call make-build.sh with appropriate arguments

#NOTE :
# 1. All script files after make-pkgs.sh need to be passed FILE_DIR_OBK as their first argument
#    this needs to be done so the scripts know where to load config.bash from
# 2. make-build.sh needs to be passed <distro name>-<DISTRO_VERSION_OBK> as 2nd argument
#    e.g. ./make-build.sh "$FILE_DIR_OBK" fedora-"$FEDORA_VERSION_OBK"

#load functions and initial vars, setup filedir, handle args
# shellcheck source=/dev/null
source function.bash
filedir_init
#DEBUG
[[ "$1" = '-v' ]] && { add_config 'DEBUG_OBK=YES' ;}
parse_args "$@"

#load vars from filedir
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash
#DEBUG
log_debug "$0"
#check what variables are loaded
#cat "$FILE_DIR_OBK"config.bash

#TOOLBOX USAGE
if [[ "$TOOLBOX_ENABLE_OBK" = 'YES' ]]; then
  #create toolbox
  echo "Creating toolbox..."

  #fedora toolbox
  if [[ "$FEDORA_OBK" = 'YES' ]]; then
    chmod +x make-fedora.sh
    chmod +x make-fedora-tweaks.sh
    #check if version string exists
    [ -z "$FEDORA_VERSION_OBK" ] && missing_version "fedora"

    echo "Setting up Fedora toolbox..."
      {
        toolbox create -y obk-toolbox-fedora-"$FEDORA_VERSION_OBK" -d fedora -r "$FEDORA_VERSION_OBK"
        #misc fedora tweaks
        toolbox run -c obk-toolbox-fedora-"$FEDORA_VERSION_OBK" ./make-fedora-tweaks.sh
        toolbox run -c obk-toolbox-fedora-"$FEDORA_VERSION_OBK" ./make-fedora.sh "$FILE_DIR_OBK"
      } || toolbox_error
  fi

  #debian toolbox
  if [[ "$DEBIAN_OBK" = 'YES' ]]; then
    chmod +x make-debian.sh
    #check if version string exists
    [ -z "$DEBIAN_VERSION_OBK" ] && missing_version "debian"

    echo "Setting up Debian toolbox..."
      (
        toolbox create -y obk-toolbox-debian-"$DEBIAN_VERSION_OBK" -i quay.io/toolbx-images/debian-toolbox:"$DEBIAN_VERSION_OBK"
        toolbox run -c obk-toolbox-debian-"$DEBIAN_VERSION_OBK" ./make-debian.sh "$FILE_DIR_OBK"
      ) || toolbox_error
  fi

#TOOLBOX NO USAGE
elif [[ "$DISTRO_COUNT" -ne 1 ]]; then
      echo "Cannot use more than one distro without --toolbox flag."
      echo "Exiting..."
      exit 1
else
  if [[ "$FEDORA_OBK" = 'YES' ]]; then
    chmod +x make-fedora.sh
    ./make-fedora.sh "$FILE_DIR_OBK"
    exit 1
  fi
  if [[ "$DEBIAN_OBK" = 'YES' ]]; then
    chmod +x make-debian.sh
    ./make-debian.sh "$FILE_DIR_OBK"
    exit 1
  fi
fi

