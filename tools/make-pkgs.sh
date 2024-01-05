#!/bin/bash
rm -r ~/.obk-build/config.bash
mkdir ~/.obk-build/
touch ~/.obk-build/config.bash

for arg in "$@"; do
    case $arg in
        --ibus)
          echo 'IM_IBUS_OBK='YES'' >> ~/.obk-build/config.bash
            ;;
        --fcitx)
          echo 'IM_FCITX_OBK='YES'' >> ~/.obk-build/config.bash 
            ;;
        --develop)
          echo 'BRANCH_OBK='develop'' >> ~/.obk-build/config.bash
            ;;
        --toolbox)
          echo 'TOOLBOX_ENABLE_OBK='YES'' >> ~/.obk-build/config.bash
            ;;
        --fedora)
          echo 'FEDORA_OBK='YES'' >> ~/.obk-build/config.bash
            ;;
        --debian)
          echo 'DEBIAN_OBK='YES'' >> ~/.obk-build/config.bash
            ;;
        *)
            echo "Invalid argument: $arg"
            exit 1
            ;;
    esac
done

#load vars from file
source ~/.obk-build/config.bash

#if user wants to use a toolbox
if [ $TOOLBOX_ENABLE_OBK = 'YES' ]; then
  #create toolbox
  echo "Creating toolbox..."
  if [ $FEDORA_OBK = 'YES' ]; then
    toolbox create obk-toolbox-fedora -d fedora
    chmod +x make-fedora.sh
    toolbox run -c obk-toolbox-fedora ./make-fedora.sh
  fi
  if [ $DEBIAN_OBK = 'YES' ]; then
    toolbox create obk-toolbox-debian -i quay.io/toolbx-images/debian-toolbox
    chmod +x make-debian.sh
    toolbox run -c obk-toolbox-debian ./make-debian.sh
  fi
#if user doesn't want ot use a toolbox
else
  if [ $FEDORA_OBK = 'YES' ]; then
    chmod +x make-fedora.sh
    ./make-fedora.sh
  fi
  if [ $DEBIAN_OBK = 'YES' ]; then
    chmod +x make-debian.sh
    ./make-debian.sh
  fi
fi

