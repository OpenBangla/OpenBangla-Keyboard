#!/bin/bash

# Variable documentation -> see function.bash
# Guide to add new distros
# 1. add distro switch to the #ARGUMENT HANDLING section of make-pkgs.sh and declare variables appropriately
# 2. add appropriate distro-specific commands to #TOOLBOX USAGE section
# 3. add appropriate distro-specific commands to #TOOLBOX NO USAGE section
# 4. add appropriate make-<DISTRO>.sh file (optional but recommended)
# 5. call make-build.sh with appropriate arguments

#load functions and initial vars, setup filedir, handle args
# shellcheck source=/dev/null
source function.bash
filedir_init
parse_args "$@"

#cat "$FILE_DIR_OBK"config.bash

#load vars from filedir
# shellcheck source=/dev/null
source "$FILE_DIR_OBK"config.bash
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
      {
        toolbox create -y obk-toolbox-debian-"$DEBIAN_VERSION_OBK" -i quay.io/toolbx-images/debian-toolbox:"$DEBIAN_VERSION_OBK"
        toolbox run -c obk-toolbox-debian-"$DEBIAN_VERSION_OBK" ./make-debian.sh "$FILE_DIR_OBK"
      } || toolbox_error
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

