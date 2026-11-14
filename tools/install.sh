#!/usr/bin/env bash
# OpenBangla Keyboard installer
#
# Installs the right OpenBangla Keyboard package for your Linux distribution and
# sets up the input method framework (IBus or Fcitx5) so you can start typing.
#
# Usage:
#   bash -c "$(curl -fsSL https://raw.githubusercontent.com/OpenBangla/OpenBangla-Keyboard/master/tools/install.sh)"
#   bash install.sh [options]
#
# Run `bash install.sh --help` for all options.

set -uo pipefail

REPO="OpenBangla/OpenBangla-Keyboard"
# The newest release on GitHub is installed. This version (with the package
# list below) is only used when GitHub can't be asked which release is newest.
FALLBACK_VERSION="2.0.0"
APP_VERSION="${OB_VERSION:-}"
WIKI_INSTALL="https://github.com/${REPO}/wiki/Installing-OpenBangla-Keyboard"
WIKI_CONFIG="https://github.com/${REPO}/wiki/Configuring-Environment"
PKG_NAME="openbangla-keyboard"
IBUS_ENGINE="OpenBangla"   # engine name in data/openbangla.xml.in
FCITX_IM="openbangla"      # input method name from src/engine/fcitx/openbangla.conf

# Packages published for ${FALLBACK_VERSION}. Used when the release page can't
# be reached; normally the list is read from the release page itself so new
# releases and uploads are picked up without editing this script.
FALLBACK_ASSETS="
OpenBangla-Keyboard_2.0.0-archlinux.pkg.tar.zst
OpenBangla-Keyboard_2.0.0-debian11.deb
OpenBangla-Keyboard_2.0.0-debian12.deb
OpenBangla-Keyboard_2.0.0-debian13.deb
OpenBangla-Keyboard_2.0.0-fedora38.rpm
OpenBangla-Keyboard_2.0.0-fedora41.rpm
OpenBangla-Keyboard_2.0.0-fedora42.rpm
OpenBangla-Keyboard_2.0.0-ubuntu20.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu22.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu24.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu25.04.deb
"

# Ubuntu codenames, so Ubuntu-based distros (Mint, Pop!_OS, Zorin, elementary,
# KDE neon, ...) can be matched through UBUNTU_CODENAME in /etc/os-release.
UBUNTU_CODENAMES="
bionic 18.04
focal 20.04
jammy 22.04
noble 24.04
oracular 24.10
plucky 25.04
questing 25.10
resolute 26.04
"

# Debian codenames, for Debian-based distros that only expose a codename.
DEBIAN_CODENAMES="
stretch 9
buster 10
bullseye 11
bookworm 12
trixie 13
forky 14
"

# Testing hooks (used by the CI tests, not needed by users):
#   OB_OS_RELEASE   path to an os-release file to read instead of /etc/os-release
#   OB_ASSETS       newline separated asset list to use instead of downloading it
#   OB_ARCH         machine architecture to assume instead of `uname -m`
#   OB_VERSION      release to install instead of the newest one
#   OB_DESKTOP      desktop name to assume instead of $XDG_CURRENT_DESKTOP
OS_RELEASE_FILE="${OB_OS_RELEASE:-/etc/os-release}"

DRY_RUN=0
ASSUME_YES=0
IM_CHOICE="auto"
DO_SETUP=1
ACTION="install"

# ---------------------------------------------------------------------------
# output helpers
# ---------------------------------------------------------------------------
if [ -t 1 ]; then
  C_RED=$'\e[31m'; C_GREEN=$'\e[32m'; C_YELLOW=$'\e[33m'; C_BOLD=$'\e[1m'; C_RESET=$'\e[0m'
else
  C_RED=""; C_GREEN=""; C_YELLOW=""; C_BOLD=""; C_RESET=""
fi
info()  { printf '%s==>%s %s\n' "$C_GREEN" "$C_RESET" "$*"; }
warn()  { printf '%sWarning:%s %s\n' "$C_YELLOW" "$C_RESET" "$*" >&2; }
error() { printf '%sError:%s %s\n' "$C_RED" "$C_RESET" "$*" >&2; }
die()   { error "$*"; exit 1; }

usage() {
  cat <<EOF
OpenBangla Keyboard installer: installs the newest release for your system.

Usage: install.sh [options]

Options:
  --ibus           Use the IBus input method framework (best for GNOME)
  --fcitx5         Use the Fcitx5 input method framework (best for KDE Plasma)
                   Without either, the installer picks one for your desktop.
  --no-setup       Only install the package; don't configure the desktop
  --uninstall      Remove OpenBangla Keyboard
  --dry-run        Show what would be done without changing anything
  -y, --yes        Don't ask for confirmation
  -h, --help       Show this help
EOF
}

# Run a command, or only print it in --dry-run mode.
run() {
  if [ "$DRY_RUN" = 1 ]; then
    printf '   [dry-run] %s\n' "$*"
    return 0
  fi
  "$@"
}

# Run a command as root (directly when already root, otherwise with sudo).
as_root() {
  if [ "$(id -u)" = 0 ]; then
    run "$@"
  elif command -v sudo >/dev/null 2>&1; then
    run sudo "$@"
  elif command -v doas >/dev/null 2>&1; then
    run doas "$@"
  else
    die "This step needs administrator rights, but neither sudo nor doas is installed. Run the installer as root."
  fi
}

# Ask a yes/no question. Reads from the terminal so it also works when the
# script itself is piped in through `bash -c "$(curl ...)"`.
confirm() {
  local answer
  [ "$ASSUME_YES" = 1 ] && return 0
  [ "$DRY_RUN" = 1 ] && return 0
  # no terminal to ask on (e.g. run from a script): go ahead
  if ! { : < /dev/tty; } 2>/dev/null; then
    return 0
  fi
  printf '%s [Y/n] ' "$1" > /dev/tty
  read -r answer < /dev/tty || answer=""
  case "$answer" in [nN]*) return 1 ;; *) return 0 ;; esac
}

# ---------------------------------------------------------------------------
# distribution detection
# ---------------------------------------------------------------------------
# Sets: OS_ID OS_NAME OS_LIKE OS_VERSION_ID OS_CODENAME OS_UBUNTU_CODENAME
read_os_release() {
  OS_ID=""; OS_NAME=""; OS_LIKE=""; OS_VERSION_ID=""; OS_CODENAME=""; OS_UBUNTU_CODENAME=""; OS_DEBIAN_CODENAME=""
  if [ -r "$OS_RELEASE_FILE" ]; then
    # Read in a subshell so the file can't overwrite our own variables.
    eval "$(
      # Clear values inherited from the environment (WSL, for example, sets
      # NAME to the computer name) so only the file's values are used.
      unset ID ID_LIKE NAME PRETTY_NAME VERSION_ID VERSION_CODENAME UBUNTU_CODENAME DEBIAN_CODENAME
      # shellcheck disable=SC1090
      . "$OS_RELEASE_FILE"
      printf 'OS_ID=%q\n' "${ID:-}"
      printf 'OS_NAME=%q\n' "${PRETTY_NAME:-${NAME:-}}"
      printf 'OS_LIKE=%q\n' "${ID_LIKE:-}"
      printf 'OS_VERSION_ID=%q\n' "${VERSION_ID:-}"
      printf 'OS_CODENAME=%q\n' "${VERSION_CODENAME:-}"
      printf 'OS_UBUNTU_CODENAME=%q\n' "${UBUNTU_CODENAME:-}"
      printf 'OS_DEBIAN_CODENAME=%q\n' "${DEBIAN_CODENAME:-}"
    )"
  elif [ -e /etc/arch-release ]; then
    # a minimal Arch install may lack /etc/os-release
    OS_ID="arch"; OS_NAME="Arch Linux"
  fi
  OS_ID=$(printf '%s' "$OS_ID" | tr '[:upper:]' '[:lower:]')
  OS_LIKE=$(printf '%s' "$OS_LIKE" | tr '[:upper:]' '[:lower:]')
}

has_word() { case " $1 " in *" $2 "*) return 0 ;; esac; return 1; }

lookup() { # lookup <table> <key> -> value
  local key value
  while read -r key value; do
    if [ -n "$key" ] && [ "$key" = "$2" ]; then printf '%s\n' "$value"; return; fi
  done <<< "$1"
}

# Sets: FAMILY (ubuntu|debian|fedora|arch|opensuse|unknown) and FAMILY_VERSION
detect_family() {
  local ids="$OS_ID $OS_LIKE"
  FAMILY="unknown"; FAMILY_VERSION=""

  if [ "$OS_ID" = "ubuntu" ]; then
    FAMILY="ubuntu"; FAMILY_VERSION="$OS_VERSION_ID"
  elif [ -n "$OS_UBUNTU_CODENAME" ] || has_word "$ids" ubuntu; then
    # Ubuntu derivative: Mint, Pop!_OS, Zorin, elementary, neon, ...
    FAMILY="ubuntu"
    FAMILY_VERSION=$(lookup "$UBUNTU_CODENAMES" "$OS_UBUNTU_CODENAME")
    if [ -z "$FAMILY_VERSION" ] && [ -r /etc/upstream-release/lsb-release ]; then
      FAMILY_VERSION=$(sed -n 's/^DISTRIB_RELEASE=//p' /etc/upstream-release/lsb-release | tr -d '"')
    fi
  elif [ "$OS_ID" = "debian" ]; then
    FAMILY="debian"; FAMILY_VERSION="${OS_VERSION_ID%%.*}"
    [ -z "$FAMILY_VERSION" ] && FAMILY_VERSION=$(lookup "$DEBIAN_CODENAMES" "$OS_CODENAME")
  elif has_word "$ids" debian; then
    # Debian derivative: Kali, Parrot, MX, LMDE, Deepin, ...
    FAMILY="debian"
    # Parrot and others name their Debian base in DEBIAN_CODENAME
    FAMILY_VERSION=$(lookup "$DEBIAN_CODENAMES" "${OS_DEBIAN_CODENAME:-$OS_CODENAME}")
    if [ -z "$FAMILY_VERSION" ] && [ -r /etc/debian_version ]; then
      # "12.5" on stable-based distros, "kali-rolling" or "trixie/sid" otherwise
      FAMILY_VERSION=$(sed -n 's/^\([0-9][0-9]*\).*/\1/p' /etc/debian_version)
    fi
    # rolling distros (Kali, Parrot, ...) follow Debian testing: newest package
    [ -z "$FAMILY_VERSION" ] && FAMILY_VERSION="latest"
  elif [ "$OS_ID" = "fedora" ]; then
    FAMILY="fedora"; FAMILY_VERSION="$OS_VERSION_ID"
  elif has_word "$ids" fedora && [ "${OS_VERSION_ID%%.*}" -ge 30 ] 2>/dev/null; then
    # Fedora remixes (Nobara, Ultramarine, ...) share Fedora's version numbers
    # (30+); RHEL clones (Rocky, Alma: versions 8-10) also list "fedora".
    FAMILY="fedora"; FAMILY_VERSION="$OS_VERSION_ID"
  elif has_word "$ids" arch || [ "$OS_ID" = "arch" ]; then
    FAMILY="arch"; FAMILY_VERSION="rolling"
  elif has_word "$ids" suse || has_word "$ids" opensuse; then
    FAMILY="opensuse"; FAMILY_VERSION="$OS_VERSION_ID"
  elif has_word "$ids" rhel || has_word "$ids" centos; then
    FAMILY="rhel"; FAMILY_VERSION="$OS_VERSION_ID"
  fi
}

# ---------------------------------------------------------------------------
# package selection
# ---------------------------------------------------------------------------
fetch() { # fetch <url> <output file or - for stdout>
  if command -v curl >/dev/null 2>&1; then
    curl -fsSL --retry 2 -o "$2" "$1"
  elif command -v wget >/dev/null 2>&1; then
    wget -q -O "$2" "$1"
  else
    return 127
  fi
}

# Minimal systems (containers, WSL, netinstall) may have neither curl nor wget.
ensure_downloader() {
  command -v curl >/dev/null 2>&1 && return
  command -v wget >/dev/null 2>&1 && return
  info "Installing curl (needed to download the package) ..."
  case "$(pkg_manager)" in
    apt)    apt_get update -qq; apt_get install -y curl ca-certificates ;;
    dnf)    as_root dnf install -y curl ;;
    pacman) as_root pacman -S --noconfirm --needed curl ;;
    zypper) as_root zypper --non-interactive install curl ;;
  esac
}

# Sets APP_VERSION to the newest release on GitHub (unless already set).
resolve_version() {
  local url=""
  if [ -z "$APP_VERSION" ] && [ -z "${OB_ASSETS:-}" ]; then
    # /releases/latest redirects to /releases/tag/<version>
    if command -v curl >/dev/null 2>&1; then
      url=$(curl -fsSL -o /dev/null -w '%{url_effective}' "https://github.com/${REPO}/releases/latest" 2>/dev/null)
    elif command -v wget >/dev/null 2>&1; then
      url=$(wget -S --spider "https://github.com/${REPO}/releases/latest" 2>&1 | sed -n 's/^ *[Ll]ocation: *\([^ ]*\).*/\1/p' | tail -n1)
    fi
    url="${url%$'\r'}"
    APP_VERSION="${url##*/tag/}"
  fi
  if ! [[ "$APP_VERSION" =~ ^[0-9]+(\.[0-9]+)*$ ]]; then
    APP_VERSION="$FALLBACK_VERSION"
  fi
  URL_STUB="https://github.com/${REPO}/releases/download/${APP_VERSION}"
  ASSETS_PAGE="https://github.com/${REPO}/releases/expanded_assets/${APP_VERSION}"
}

load_assets() {
  if [ -n "${OB_ASSETS:-}" ]; then
    ASSETS="$OB_ASSETS"
    return
  fi
  # Package names: "OpenBangla-Keyboard_2.0.0-ubuntu24.04.deb" (2.x) and
  # "openbangla-keyboard_3.0.0-ubuntu24.04_amd64.deb" (3.x and later).
  ASSETS=$(fetch "$ASSETS_PAGE" - 2>/dev/null \
    | grep -oiE "openbangla-keyboard_${APP_VERSION//./\\.}-[A-Za-z0-9._-]+\.(deb|rpm|pkg\.tar\.zst)" \
    | sort -u)
  if [ -z "$ASSETS" ]; then
    if [ "$APP_VERSION" != "$FALLBACK_VERSION" ]; then
      die "Couldn't read the list of OpenBangla Keyboard ${APP_VERSION} packages from GitHub. Check your internet connection and try again."
    fi
    warn "Couldn't read the list of packages from GitHub; using the built-in list."
    ASSETS="$FALLBACK_ASSETS"
  fi
}

# Normalised processor architecture: x86_64, aarch64 or whatever uname says.
machine_arch() {
  case "${OB_ARCH:-$(uname -m)}" in
    x86_64|amd64)  echo x86_64 ;;
    aarch64|arm64) echo aarch64 ;;
    *) echo "${OB_ARCH:-$(uname -m)}" ;;
  esac
}

# Architecture a package is built for. 2.x package names don't say; they are
# all x86_64.
asset_arch() {
  case "$1" in
    *amd64*|*x86_64*)  echo x86_64 ;;
    *arm64*|*aarch64*) echo aarch64 ;;
    *armhf*|*armv7*|*i386*|*i686*|*ppc64*|*s390x*|*riscv64*|*loongarch*) echo other ;;
    *) echo x86_64 ;;
  esac
}

# Keep only the packages built for this computer's architecture.
# Sets ASSETS and ASSET_ARCHES (all architectures the release has packages for).
filter_assets_by_arch() {
  local arch line a kept=""
  arch=$(machine_arch)
  ASSET_ARCHES=""
  while IFS= read -r line; do
    [ -z "$line" ] && continue
    a=$(asset_arch "$line")
    has_word "$ASSET_ARCHES" "$a" || ASSET_ARCHES="${ASSET_ARCHES:+$ASSET_ARCHES }$a"
    [ "$a" = "$arch" ] && kept="${kept}${line}"$'\n'
  done <<< "$ASSETS"
  ASSETS="$kept"
}

# Print "<version> <file>" for every package of the given family, newest first.
family_assets() {
  # (plain bash: minimal systems such as Fedora containers have no awk)
  local prefix="openbangla-keyboard_${APP_VERSION}-$1" line rest
  while IFS= read -r line; do
    [[ "${line,,}" == "$prefix"* ]] || continue
    rest="${line:${#prefix}}"
    # "24.04.deb", "13.deb", "10-buster.deb", "42.rpm", "24.04_amd64.deb",
    # "41.x86_64.rpm"
    [[ "$rest" =~ ^([0-9]+(\.[0-9]+)?) ]] || continue
    printf '%s %s\n' "${BASH_REMATCH[1]}" "$line"
  done <<< "$ASSETS" | sort -t' ' -k1,1Vr
}

# Version comparison: returns 0 if $1 <= $2
version_le() { [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | head -n1)" = "$1" ]; }

# Sets: PACKAGE_FILE and PACKAGE_NOTE
choose_package() {
  local list newest best="" v f
  PACKAGE_FILE=""; PACKAGE_NOTE=""

  case "$FAMILY" in
    arch)
      PACKAGE_FILE=$(printf '%s\n' "$ASSETS" | grep -m1 -i -- '-archlinux.*\.pkg\.tar\.zst$')
      return
      ;;
    opensuse)
      # Only Tumbleweed (and Slowroll, which follows it) has a package.
      case "$OS_ID" in
        *tumbleweed*|*slowroll*)
          PACKAGE_FILE=$(printf '%s\n' "$ASSETS" | grep -m1 -i -- '-opensuse-tumbleweed.*\.rpm$') ;;
        *) [ -n "$(printf '%s\n' "$ASSETS" | grep -i -- '-opensuse-')" ] \
             && PACKAGE_NOTE="Only openSUSE Tumbleweed has a ready-made package; ${OS_NAME:-this version} isn't supported yet." ;;
      esac
      [ -z "$PACKAGE_FILE" ] && [ -z "$PACKAGE_NOTE" ] && PACKAGE_NOTE="There's no ready-made package for openSUSE in OpenBangla Keyboard ${APP_VERSION}."
      return
      ;;
    ubuntu|debian|fedora) ;;
    *) return ;;
  esac

  list=$(family_assets "$FAMILY")
  [ -z "$list" ] && return
  newest=$(printf '%s\n' "$list" | head -n1 | cut -d' ' -f1)

  if [ -z "$FAMILY_VERSION" ] || [ "$FAMILY_VERSION" = "latest" ]; then
    PACKAGE_FILE=$(printf '%s\n' "$list" | head -n1 | cut -d' ' -f2)
    [ -z "$FAMILY_VERSION" ] && PACKAGE_NOTE="Couldn't tell your ${FAMILY} version; using the newest package (${FAMILY} ${newest})."
    return
  fi

  # newest package whose version is <= the system's version
  while read -r v f; do
    if version_le "$v" "$FAMILY_VERSION"; then best="$f"; break; fi
  done <<< "$list"

  if [ -z "$best" ]; then
    PACKAGE_NOTE="${FAMILY} ${FAMILY_VERSION} is older than any published package."
    return
  fi
  PACKAGE_FILE="$best"
  # $v still holds the version of the package picked by the loop above
  if [ "$v" != "$FAMILY_VERSION" ]; then
    PACKAGE_NOTE="There's no package built for ${FAMILY} ${FAMILY_VERSION} yet; using the one for ${FAMILY} ${v}, which usually works."
  fi
}

# ---------------------------------------------------------------------------
# package manager
# ---------------------------------------------------------------------------
# apt without interactive questions (e.g. "restart services?" during library
# upgrades), keeping existing config files, and waiting up to 5 minutes if the
# automatic updater is holding the package database lock.
apt_get() {
  as_root env DEBIAN_FRONTEND=noninteractive NEEDRESTART_MODE=a apt-get     -o Dpkg::Options::=--force-confdef -o Dpkg::Options::=--force-confold     -o DPkg::Lock::Timeout=300 "$@"
}

pkg_manager() {
  case "$FAMILY" in
    ubuntu|debian) echo apt ;;
    fedora) echo dnf ;;
    arch) echo pacman ;;
    opensuse) echo zypper ;;
    *) echo unknown ;;
  esac
}

install_local_package() { # install_local_package <file>
  case "$(pkg_manager)" in
    apt)    apt_get update -qq; apt_get install -y "$1" ;;
    dnf)    as_root dnf install -y "$1" ;;
    pacman) as_root pacman -U --noconfirm --needed "$1" ;;
    # the release packages aren't signed with a key zypper knows
    zypper) as_root zypper --non-interactive install --allow-unsigned-rpm "$1" ;;
  esac
}

# Install packages from the distro's own repositories; names that don't exist
# on this distro are skipped instead of failing the whole install.
install_repo_packages() {
  local p
  for p in "$@"; do
    case "$(pkg_manager)" in
      apt)    apt_get install -y "$p" >/dev/null 2>&1 || warn "Package '$p' not available, skipping." ;;
      dnf)    as_root dnf install -y "$p" >/dev/null 2>&1 || warn "Package '$p' not available, skipping." ;;
      pacman) as_root pacman -S --noconfirm --needed "$p" >/dev/null 2>&1 || warn "Package '$p' not available, skipping." ;;
      zypper) as_root zypper --non-interactive install "$p" >/dev/null 2>&1 || warn "Package '$p' not available, skipping." ;;
    esac
  done
}

# True if the installed OpenBangla package contains the Fcitx5 plugin.
# (The 2.0.0 release packages are built with IBus support only.)
package_has_fcitx5() {
  [ "$DRY_RUN" = 1 ] && return 1
  case "$(pkg_manager)" in
    apt)        dpkg -L "$PKG_NAME" 2>/dev/null | grep -q '/fcitx5/' ;;
    dnf|zypper) rpm -ql "$PKG_NAME" 2>/dev/null | grep -q '/fcitx5/' ;;
    pacman)     pacman -Qlq "$PKG_NAME" 2>/dev/null | grep -q '/fcitx5/' ;;
    *) return 1 ;;
  esac
}

is_installed() {
  case "$(pkg_manager)" in
    apt)    dpkg-query -W -f='${Status}' "$PKG_NAME" 2>/dev/null | grep -q "install ok installed" ;;
    dnf|zypper) rpm -q "$PKG_NAME" >/dev/null 2>&1 ;;
    pacman) pacman -Qq "$PKG_NAME" >/dev/null 2>&1 ;;
    *) return 1 ;;
  esac
}

# ---------------------------------------------------------------------------
# input method setup
# ---------------------------------------------------------------------------
# The user whose desktop we configure (the real user even under sudo).
target_user() {
  if [ "$(id -u)" = 0 ] && [ -n "${SUDO_USER:-}" ] && [ "$SUDO_USER" != root ]; then
    echo "$SUDO_USER"
  else
    id -un
  fi
}

target_home() { getent passwd "$(target_user)" | cut -d: -f6; }

# Run a command as the desktop user (needed for gsettings and config files).
as_user() {
  local u; u=$(target_user)
  if [ "$(id -un)" = "$u" ]; then run "$@"; else run sudo -u "$u" "$@"; fi
}

desktop_name() {
  printf '%s' "${OB_DESKTOP:-${XDG_CURRENT_DESKTOP:-${DESKTOP_SESSION:-}}}" | tr '[:upper:]' '[:lower:]'
}

# OpenBangla Keyboard 3.0 and later set up the desktop themselves
# (`openbangla-gui --setup-system`, src/frontend/PlatformConfig.cpp).
app_sets_up_desktop() { ! version_le "$APP_VERSION" 2.99; }

choose_framework() {
  local de
  if [ "$IM_CHOICE" != auto ]; then FRAMEWORK="$IM_CHOICE"; return; fi
  de=$(desktop_name)
  if pgrep -x fcitx5 >/dev/null 2>&1; then
    FRAMEWORK=fcitx5; FRAMEWORK_REASON="Fcitx5 is already running"
  elif pgrep -x ibus-daemon >/dev/null 2>&1; then
    FRAMEWORK=ibus; FRAMEWORK_REASON="IBus is already running"
  elif [[ "$de" == *gnome* || "$de" == *zorin* || "$de" == *unity* || "$de" == *pantheon* || "$de" == *cinnamon* ]]; then
    FRAMEWORK=ibus; FRAMEWORK_REASON="it is built into your desktop (${de})"
  elif [[ "$de" == *budgie* ]] && ! app_sets_up_desktop; then
    FRAMEWORK=ibus; FRAMEWORK_REASON="it is built into your desktop (${de})"
  elif [[ "$de" == *kde* || "$de" == *plasma* || "$de" == *hyprland* || "$de" == *sway* || "$de" == *wlroots* ]]; then
    FRAMEWORK=fcitx5; FRAMEWORK_REASON="it works best on your desktop (${de})"
  elif app_sets_up_desktop && [[ "$de" == *xfce* || "$de" == *mate* || "$de" == *budgie* || "$de" == *lxqt* || "$de" == *deepin* ]]; then
    # the same choice OpenBangla Keyboard 3 makes for these desktops
    FRAMEWORK=fcitx5; FRAMEWORK_REASON="OpenBangla Keyboard uses it on your desktop (${de})"
  else
    FRAMEWORK=ibus; FRAMEWORK_REASON="it is the most widely supported default"
  fi
}

# True when the desktop user's graphical session can be reached.
has_desktop_session() {
  [ -n "${DISPLAY:-}${WAYLAND_DISPLAY:-}" ] && [ -n "${DBUS_SESSION_BUS_ADDRESS:-}" ]
}

# Let OpenBangla Keyboard 3+ add itself to the desktop's input sources.
# Returns non-zero if it couldn't, so the installer can do it instead.
run_app_setup() {
  local bin="" b
  for b in /usr/bin/openbangla-gui /usr/local/bin/openbangla-gui; do
    [ -x "$b" ] && { bin="$b"; break; }
  done
  if [ "$DRY_RUN" = 1 ]; then
    printf '   [dry-run] openbangla-gui --setup-system\n'
    return 0
  fi
  [ -n "$bin" ] && has_desktop_session || return 1
  info "OpenBangla Keyboard is adding itself to your input sources ..."
  # keep the session variables, which sudo -u would drop
  as_user env DISPLAY="${DISPLAY:-}" WAYLAND_DISPLAY="${WAYLAND_DISPLAY:-}" \
    XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-}" DBUS_SESSION_BUS_ADDRESS="${DBUS_SESSION_BUS_ADDRESS:-}" \
    XDG_CURRENT_DESKTOP="${XDG_CURRENT_DESKTOP:-}" DESKTOP_SESSION="${DESKTOP_SESSION:-}" \
    timeout 60 "$bin" --setup-system >/dev/null 2>&1
}

framework_packages() {
  case "$FRAMEWORK:$(pkg_manager)" in
    ibus:apt)      echo "ibus ibus-gtk3 ibus-gtk4" ;;
    ibus:dnf)      echo "ibus ibus-gtk3" ;;
    ibus:pacman)   echo "ibus" ;;
    fcitx5:apt)    echo "fcitx5 fcitx5-frontend-gtk3 fcitx5-frontend-gtk4 fcitx5-frontend-qt5 fcitx5-config-qt" ;;
    fcitx5:dnf)    echo "fcitx5 fcitx5-gtk fcitx5-qt fcitx5-configtool" ;;
    fcitx5:pacman) echo "fcitx5 fcitx5-gtk fcitx5-qt fcitx5-configtool" ;;
    ibus:zypper)   echo "ibus ibus-gtk3" ;;
    fcitx5:zypper) echo "fcitx5 fcitx5-gtk3 fcitx5-qt5 fcitx5-configtool" ;;
  esac
}

write_user_file() { # write_user_file <path> <content>
  if [ "$DRY_RUN" = 1 ]; then
    printf '   [dry-run] write %s:\n' "$1"; printf '%s\n' "$2" | sed 's/^/             /'
    return
  fi
  as_user mkdir -p "$(dirname "$1")"
  printf '%s\n' "$2" | as_user tee "$1" >/dev/null
}

# Which input method framework the user has already set up, if any:
# prints "ibus", "fcitx5", "fcitx" (v4), "other" or nothing.
existing_framework() {
  local home val f
  home=$(target_home)
  if pgrep -x fcitx5 >/dev/null 2>&1; then echo fcitx5; return; fi
  if pgrep -x fcitx >/dev/null 2>&1; then echo fcitx; return; fi
  val="${GTK_IM_MODULE:-}${XMODIFIERS:-}"
  # settings in the usual places, ignoring the file this installer writes
  for f in "$home/.xinputrc" "$home/.pam_environment" "$home/.profile" "$home/.xprofile"            "$home/.config/environment.d/"*.conf /etc/environment /etc/environment.d/*.conf; do
    [ -r "$f" ] || continue
    case "$f" in *90-openbangla-keyboard.conf) continue ;; esac
    val="$val $(grep -h -o -E '(GTK_IM_MODULE|XMODIFIERS|QT_IM_MODULE)=[^ ]*|run_im [a-z0-9]+' "$f" 2>/dev/null | tr '
' ' ')"
  done
  case "$val" in
    *fcitx5*) echo fcitx5 ;;
    *fcitx*)  echo fcitx ;;
    *ibus*)   echo ibus ;;
    *uim*|*scim*|*kime*|*nimf*|*hime*|*gcin*) echo other ;;
  esac
}

setup_environment() {
  local home env_file content current
  home=$(target_home)
  current=$(existing_framework)
  if [ -n "$current" ] && [ "$current" != "$FRAMEWORK" ]; then
    # Never take over an input method the user already relies on
    # (e.g. Fcitx for Chinese/Japanese input).
    KEPT_FRAMEWORK="$current"
    return
  fi
  if [ "$(pkg_manager)" = apt ] && command -v im-config >/dev/null 2>&1; then
    # Debian/Ubuntu's own tool for choosing the input method
    as_user im-config -n "$FRAMEWORK"
  fi
  # Also write systemd's environment.d, read by GNOME, KDE and most desktops.
  # (GNOME on Wayland ignores GTK_IM_MODULE for IBus, so this is harmless there.)
  env_file="$home/.config/environment.d/90-openbangla-keyboard.conf"
  if [ "$FRAMEWORK" = fcitx5 ]; then
    content="# Added by the OpenBangla Keyboard installer
XMODIFIERS=@im=fcitx
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
SDL_IM_MODULE=fcitx"
  else
    content="# Added by the OpenBangla Keyboard installer
XMODIFIERS=@im=ibus
GTK_IM_MODULE=ibus
QT_IM_MODULE=ibus"
  fi
  write_user_file "$env_file" "$content"
  NEEDS_RELOGIN=1
}

setup_ibus() {
  local de current new
  de=$(desktop_name)
  if [[ "$de" == *gnome* ]] && command -v gsettings >/dev/null 2>&1; then
    current=$(as_user gsettings get org.gnome.desktop.input-sources sources 2>/dev/null || true)
    if [[ "$current" == *"'ibus', '${IBUS_ENGINE}'"* ]]; then
      info "OpenBangla is already in your GNOME input sources."
    else
      if [ -z "$current" ] || [ "$current" = "@a(ss) []" ] || [ "$current" = "[]" ]; then
        new="[('xkb', 'us'), ('ibus', '${IBUS_ENGINE}')]"
      else
        new="${current%]}, ('ibus', '${IBUS_ENGINE}')]"
      fi
      info "Adding OpenBangla Keyboard to your GNOME input sources."
      as_user gsettings set org.gnome.desktop.input-sources sources "$new"
      ADDED_SOURCE=1
    fi
  else
    setup_environment
    if command -v ibus >/dev/null 2>&1 && pgrep -x ibus-daemon >/dev/null 2>&1; then
      as_user ibus restart >/dev/null 2>&1 || true
    fi
  fi
}

setup_fcitx5() {
  local home profile autostart
  home=$(target_home)
  profile="$home/.config/fcitx5/profile"
  setup_environment

  # Start Fcitx5 automatically on desktops that don't do it themselves.
  autostart="$home/.config/autostart/org.fcitx.Fcitx5.desktop"
  if [ ! -e "$autostart" ] && [ -e /usr/share/applications/org.fcitx.Fcitx5.desktop ]; then
    as_user mkdir -p "$home/.config/autostart"
    as_user cp /usr/share/applications/org.fcitx.Fcitx5.desktop "$autostart"
  fi

  if [ -e "$profile" ] && grep -q "^Name=${FCITX_IM}\$" "$profile"; then
    info "OpenBangla is already in your Fcitx5 input methods."
  elif [ ! -e "$profile" ] && ! pgrep -x fcitx5 >/dev/null 2>&1; then
    # No profile yet and Fcitx5 isn't running: create one with English + Bangla.
    write_user_file "$profile" "[Groups/0]
Name=Default
Default Layout=us
DefaultIM=keyboard-us

[Groups/0/Items/0]
Name=keyboard-us
Layout=

[Groups/0/Items/1]
Name=${FCITX_IM}
Layout=

[GroupOrder]
0=Default"
    ADDED_SOURCE=1
  else
    # Fcitx5 rewrites its profile while running, so let the user add it.
    MANUAL_FCITX_STEP=1
  fi
}

# ---------------------------------------------------------------------------
# actions
# ---------------------------------------------------------------------------
setup_desktop() {
  local current
  current=$(existing_framework)
  if [ -n "$current" ] && [ "$current" != "$FRAMEWORK" ]; then
    # Never take over an input method the user already relies on.
    KEPT_FRAMEWORK="$current"
    return
  fi
  if app_sets_up_desktop && [ "$IM_CHOICE" = auto ] && run_app_setup; then
    APP_DID_SETUP=1; ADDED_SOURCE=1
    [[ "$(desktop_name)" == *gnome* ]] || NEEDS_RELOGIN=1
    return
  fi
  # OpenBangla Keyboard 2.x, a framework chosen with --ibus/--fcitx5, or the
  # app couldn't reach the desktop session: do it here.
  if [ "$FRAMEWORK" = fcitx5 ]; then setup_fcitx5; else setup_ibus; fi
}

print_header() {
  cat <<EOF
${C_BOLD}OpenBangla Keyboard installer${C_RESET}
EOF
}

unsupported() {
  error "$1"
  cat <<EOF >&2

You can still install OpenBangla Keyboard by building it from source:
  https://github.com/${REPO}#building-from-source
Distro-specific instructions:
  ${WIKI_INSTALL}
If you think this distro should be supported, please open an issue and include
the output of:  cat /etc/os-release
  https://github.com/${REPO}/issues
EOF
  exit 1
}

do_uninstall() {
  if ! is_installed; then
    info "OpenBangla Keyboard is not installed."
    return
  fi
  confirm "Remove OpenBangla Keyboard?" || exit 0
  case "$(pkg_manager)" in
    apt)    apt_get remove -y "$PKG_NAME" ;;
    dnf)    as_root dnf remove -y "$PKG_NAME" ;;
    pacman) as_root pacman -R --noconfirm "$PKG_NAME" ;;
    zypper) as_root zypper --non-interactive remove "$PKG_NAME" ;;
  esac
  local f; f="$(target_home)/.config/environment.d/90-openbangla-keyboard.conf"
  [ -e "$f" ] && run rm -f "$f"
  info "OpenBangla Keyboard has been removed."
}

do_install() {
  local arch file
  arch=$(machine_arch)

  case "$FAMILY" in
    rhel)     unsupported "There's no ready-made package for ${OS_NAME:-RHEL-based distros} yet." ;;
    unknown)  unsupported "Couldn't recognise your Linux distribution (${OS_NAME:-unknown})." ;;
  esac

  [ "$DRY_RUN" = 0 ] && ensure_downloader
  resolve_version
  load_assets
  filter_assets_by_arch
  if [ -z "$ASSETS" ]; then
    unsupported "OpenBangla Keyboard ${APP_VERSION} has no ready-made packages for your computer's processor (${arch}); packages are available for: ${ASSET_ARCHES:-x86_64}."
  fi
  choose_package
  [ -z "$PACKAGE_FILE" ] && unsupported "${PACKAGE_NOTE:-No package is available for ${OS_NAME}.}"

  choose_framework
  info "Distribution:   ${OS_NAME:-$OS_ID} (${FAMILY} ${FAMILY_VERSION})"
  info "Version:        OpenBangla Keyboard ${APP_VERSION}"
  info "Package:        ${PACKAGE_FILE}"
  [ -n "$PACKAGE_NOTE" ] && warn "$PACKAGE_NOTE"
  if [ "$DO_SETUP" = 1 ]; then
    info "Input method:   ${FRAMEWORK} (${FRAMEWORK_REASON:-chosen by you})"
  fi
  echo
  confirm "Continue?" || exit 0

  TMP_DIR=$(mktemp -d)
  # the package manager may run as an unprivileged user (_apt), so the
  # download must be readable by everyone
  chmod 755 "$TMP_DIR"
  trap 'rm -rf "${TMP_DIR:-}"' EXIT
  file="$TMP_DIR/$PACKAGE_FILE"

  info "Downloading ${PACKAGE_FILE} ..."
  if [ "$DRY_RUN" = 1 ]; then
    printf '   [dry-run] download %s/%s\n' "$URL_STUB" "$PACKAGE_FILE"
  else
    fetch "$URL_STUB/$PACKAGE_FILE" "$file" || die "Download failed. Check your internet connection and try again."
    chmod 644 "$file"
  fi

  info "Installing OpenBangla Keyboard ..."
  install_local_package "$file" || die "The package manager couldn't install OpenBangla Keyboard (see the messages above)."

  if [ "$DO_SETUP" = 1 ] && [ "$FRAMEWORK" = fcitx5 ] && ! package_has_fcitx5; then
    if [ "$DRY_RUN" = 1 ]; then
      warn "Fcitx5 will only be used if the package includes the Fcitx5 plugin; otherwise IBus is set up instead."
    else
      warn "This OpenBangla package only supports IBus, so IBus will be set up instead of Fcitx5."
      FRAMEWORK=ibus
    fi
  fi

  if [ "$DO_SETUP" = 1 ]; then
    info "Installing and setting up ${FRAMEWORK} ..."
    # shellcheck disable=SC2046
    install_repo_packages $(framework_packages)
    if [ "$(id -u)" = 0 ] && [ "$(target_user)" = root ]; then
      warn "Running as root without sudo: skipping desktop setup for the root account."
      warn "Run the installer again as your normal user (it will use sudo) to set up your desktop."
      DO_SETUP=0
    else
      setup_desktop
    fi
  fi

  echo
  if [ "$DRY_RUN" = 1 ]; then
    info "${C_BOLD}Dry run finished; nothing was changed.${C_RESET}"
  else
    info "${C_BOLD}OpenBangla Keyboard is installed.${C_RESET}"
  fi
  if [ "$DO_SETUP" = 1 ]; then
    if [ "${APP_DID_SETUP:-0}" = 1 ]; then
      echo "   * OpenBangla Keyboard has added itself to your input sources."
    elif [ "${ADDED_SOURCE:-0}" = 1 ]; then
      echo "   * OpenBangla Keyboard has been added to your input sources."
    fi
    if [ "${NEEDS_RELOGIN:-0}" = 1 ]; then
      echo "   * Log out and log back in (or restart) so the input method starts."
    fi
    if [ -n "${KEPT_FRAMEWORK:-}" ]; then
      warn "You already use ${KEPT_FRAMEWORK} as your input method, so your settings were left unchanged."
      echo "   * To type Bangla with OpenBangla, switch your input method to ${FRAMEWORK}"
      echo "     (on Ubuntu/Debian: run 'im-config' and choose ${FRAMEWORK}), then log out and back in."
    fi
    if [ "${MANUAL_FCITX_STEP:-0}" = 1 ]; then
      echo "   * Open 'Fcitx 5 Configuration', find 'OpenBangla Keyboard' and add it"
      echo "     to the list on the left."
    fi
    if [[ "$(desktop_name)" == *kde* || "$(desktop_name)" == *plasma* ]] && [ "$FRAMEWORK" = fcitx5 ]; then
      echo "   * On Plasma (Wayland): System Settings > Keyboard > Virtual Keyboard > choose 'Fcitx 5'."
    fi
    if [ -z "${KEPT_FRAMEWORK:-}" ]; then
      if [ "$FRAMEWORK" = ibus ]; then
        echo "   * Switch between English and Bangla with Super+Space."
      else
        echo "   * Switch between English and Bangla with Ctrl+Space."
      fi
    fi
  else
    echo "   Set up your desktop to use it: ${WIKI_CONFIG}"
  fi
}

main() {
  while [ $# -gt 0 ]; do
    case "$1" in
      --ibus) IM_CHOICE=ibus ;;
      --fcitx5|--fcitx) IM_CHOICE=fcitx5 ;;
      --no-setup) DO_SETUP=0 ;;
      --uninstall) ACTION=uninstall ;;
      --dry-run) DRY_RUN=1 ;;
      -y|--yes) ASSUME_YES=1 ;;
      -h|--help) usage; exit 0 ;;
      *) error "Unknown option: $1"; usage >&2; exit 2 ;;
    esac
    shift
  done

  print_header
  read_os_release
  detect_family
  if [ "$ACTION" = uninstall ]; then do_uninstall; else do_install; fi
}

main "$@"
