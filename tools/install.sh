#!/bin/bash
## MUST: Always update this whenever a new version is released. Also remember to update download URLs if needed
APP_VERSION=2.0.0
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
  distro_data=$(for n in /etc/*-release; do [ ! -d "$n" ]&&cat "$n"; done)
  while read -r line; do
      if [[ $line =~ ^(NAME|DISTRIB_ID|ID|ID_LIKE)=(.+)$ ]]; then
      distro_str=${BASH_REMATCH[2]}
      if echo "$distro_str" | grep -qP '(?i).*(ubuntu|mint|zorin|elementary|vanilla).*'; then
        distro_name='ubuntu'
      elif echo "$distro_str" | grep -qP '(?i).*(fedora|bazzite|bluefin|nobara|sericea|onyx|aurora).*'; then
        distro_name='fedora'
      elif echo "$distro_str" | grep -qP '(?i)^[''"]?(arch|manjaro|steamos|antergos|chakra|magpie|parabola|anarchy).*'; then
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

# detect immutable / read-only filesystem
function is_immutable {
  if [ -f /etc/os-release ]; then
    # shellcheck disable=SC1091
    source /etc/os-release
    
    # for known immutable variants (fedora silverblue, bazzite family, etc.)
    case "$VARIANT_ID" in
      silverblue|kinoite|bazzite*|bluefin|sericea|onyx|coreos|aurora*) return 0 ;;
    esac
    
    # for base immutable distributions (steamos, vanilla os)
    case "$ID" in
      steamos|vanilla) return 0 ;;
    esac
  fi

  # runtime tooling & boot flags as fallback
  [ -f /run/ostree-booted ] && return 0
  command -v bootc >/dev/null 2>&1 && return 0
  command -v rpm-ostree >/dev/null 2>&1 && return 0
  command -v steamos-readonly >/dev/null 2>&1 && return 0
  command -v transactional-update >/dev/null 2>&1 && return 0
  command -v abroot >/dev/null 2>&1 && return 0

  # kernel vfs mount check as fallback
  if command -v findmnt >/dev/null 2>&1; then
    local usr_opts
    usr_opts=$(findmnt -n -o OPTIONS -T /usr 2>/dev/null)
    if echo "$usr_opts" | grep -Eq '(^|,)ro($|,)'; then
      return 0
    fi
  fi

  return 1
}


# perform userspace installation to ~/.local
function install_userspace {
  local distro=$1 target_dir="$HOME/.local" tmp_dir xml_path pkg_url engine_bin
  tmp_dir=$(mktemp -d /tmp/openbangla-user.XXXXXX)

  echo "Immutable OS or --user requested. Installing into $target_dir ..."
  mkdir -p "$target_dir"

  case $distro in
  ("fedora")
    # verify required extraction tools
    if ! command -v rpm2cpio >/dev/null 2>&1 || ! command -v cpio >/dev/null 2>&1; then
      echo "Error: 'rpm2cpio' and 'cpio' are required to extract RPM packages in user space."
      echo "Please install them (e.g. sudo dnf install cpio rpm) and run this script again."
      rm -rf "$tmp_dir"
      exit 1
    fi

    # detect version with fallback for future fedora releases (e.g. 44, 45)
    [ -f /etc/os-release ] && source /etc/os-release
    local f_ver="$VERSION_ID"
    local check_url="${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-fedora${f_ver}.rpm"
    if ! curl -fsI "$check_url" >/dev/null 2>&1; then
      f_ver="42"
    fi

    pkg_url="${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-fedora${f_ver}.rpm"
    echo "Downloading $pkg_url ..."
    curl -# -L "$pkg_url" -o "$tmp_dir/package.rpm"
    (cd "$tmp_dir" && rpm2cpio package.rpm | cpio -idmv >/dev/null 2>&1)
    ;;

  ("arch")
    # arch and steamos are rolling releases (single universal package)
    pkg_url="${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-archlinux.pkg.tar.zst"
    echo "Downloading $pkg_url ..."
    curl -# -L "$pkg_url" -o "$tmp_dir/package.pkg.tar.zst"
    (cd "$tmp_dir" && tar -xf package.pkg.tar.zst)
    ;;

  ("ubuntu"|*)
    [ -f /etc/os-release ] && source /etc/os-release
    local u_ver="${VERSION_ID%%.*}.04"
    local check_url="${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-ubuntu${u_ver}.deb"
    if ! curl -fsI "$check_url" >/dev/null 2>&1; then
      u_ver="24.04"
    fi

    pkg_url="${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-ubuntu${u_ver}.deb"
    echo "Downloading $pkg_url ..."
    curl -# -L "$pkg_url" -o "$tmp_dir/package.deb"
    (cd "$tmp_dir" && ar -x package.deb && tar -xf data.tar.* 2>/dev/null)
    ;;
  esac

  # copy files to ~/.local
  if [ -d "$tmp_dir/usr" ]; then
    cp -rn "$tmp_dir/usr/"* "$target_dir/"
  else
    echo "Failed to extract package files."
    rm -rf "$tmp_dir"
    exit 1
  fi
  rm -rf "$tmp_dir"

  # dynamically patch executable path in ibus component xml
  xml_path="$target_dir/share/ibus/component/openbangla.xml"
  if [ -f "$xml_path" ]; then
    engine_bin=$(find "$target_dir" -name "ibus*openbangla*" -type f | head -n 1)
    if [ -n "$engine_bin" ]; then
      sed -i -E "s|<exec>[^ ]+|<exec>$engine_bin|g" "$xml_path"
    fi
  fi

  # setup systemd environment so ibus discovers ~/.local components
  mkdir -p "$HOME/.config/environment.d"
  cat << 'ENVCONF' > "$HOME/.config/environment.d/10-openbangla.conf"
IBUS_COMPONENT_PATH="${HOME}/.local/share/ibus/component:/usr/share/ibus/component"
XDG_DATA_DIRS="${HOME}/.local/share:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"
ENVCONF

  # regenerate ibus cache and restart daemon
  export IBUS_COMPONENT_PATH="$target_dir/share/ibus/component:/usr/share/ibus/component"
  ibus write-cache >/dev/null 2>&1
  ibus restart >/dev/null 2>&1

  echo "Installation completed successfully in $target_dir."
  echo "Please log out and log back in, then add OpenBangla Keyboard from your desktop settings."
}

DISTRO_NAME=$(get_distro)

# check if userspace installation is required or requested
if [ "$1" = "--user" ] || is_immutable; then
  install_userspace "$DISTRO_NAME"
  exit 0
fi

echo "Downloading installation package for $DISTRO_NAME ..."
# start downloading and install
case $DISTRO_NAME in
("ubuntu")
  # VERSION_ID (and some more vars) are sourced
  # shellcheck disable=SC1091
  source /etc/os-release
  if [ $ID = "linuxmint" ]; then
   	VERSION_ID=$(grep DISTRIB_RELEASE /etc/upstream-release/lsb-release|cut -d= -f2)
  fi
	  case ${VERSION_ID%%.*} in
	    25|24|23|22|21|20|18) VERSION_ID=${VERSION_ID%%.*}.04;UBUNTU_SUPPORTED=1;;
	    *) 
	      echo "Newer/unrecognized Ubuntu release \"$VERSION_ID\" detected. Falling back to 24.04..."
	      VERSION_ID="24.04"
	      UBUNTU_SUPPORTED=1
	      ;;
	  esac

  if [[ $UBUNTU_SUPPORTED = 1 ]]; then
    wget -q --show-progress "${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-ubuntu${VERSION_ID}.deb" -O "$HOME/OpenBangla.deb"
    sudo apt install "$HOME/OpenBangla.deb"
    mv -f "$HOME/OpenBangla.deb" /tmp/
  else
    echo "Ubuntu version not supported ($VERSION_ID)."
  fi
  ;;
("arch")
  wget -q --show-progress "${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-archlinux.pkg.tar.zst" -O /tmp/OpenBangla.pkg.tar.zst
  sudo pacman -U /tmp/OpenBangla.pkg.tar.zst
  ;;
("fedora")
  # fedora version check ?
  wget -q  "${URL_STUB}/OpenBangla-Keyboard_${APP_VERSION}-fedora38.rpm" -O /tmp/OpenBangla.rpm
  sudo dnf install /tmp/OpenBangla.rpm
  ;;
(*)
  echo
  echo "Sorry, this script was not able to identify your OS distribution."
  echo "Please report a bug at https://github.com/OpenBangla/OpenBangla-Keyboard/issues"
  echo
  echo "Please visit https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard for distrowise/distro-specific install instructions."
  echo

  echo -n "Do you want to see the instructions? (Yes/No) "
  read -r answer
  case $answer in
  [yY]*)
    URL="https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing-OpenBangla-Keyboard"
    [[ -x $BROWSER ]] && exec "$BROWSER" "$URL"
    path=$(command -v xdg-open || command -v gnome-open) && exec "$path" "$URL"
    echo "Can't find browser"
    ;;
  [nN]*)
    exit
    ;;
  esac
  ;;
esac
