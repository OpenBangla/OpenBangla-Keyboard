#!/usr/bin/env bash
# Tests for tools/install.sh distribution detection and package selection.
#
# Each case feeds the installer a fake /etc/os-release (through OB_OS_RELEASE)
# and a fixed list of release packages (OB_ASSETS), runs it with --dry-run and
# checks which package it picks. Nothing is downloaded or installed.
#
# Usage: bash tools/test-install.sh

set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
INSTALLER="$HERE/install.sh"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

export OB_ASSETS="OpenBangla-Keyboard_2.0.0-archlinux.pkg.tar.zst
OpenBangla-Keyboard_2.0.0-debian10-buster.deb
OpenBangla-Keyboard_2.0.0-debian11.deb
OpenBangla-Keyboard_2.0.0-debian12.deb
OpenBangla-Keyboard_2.0.0-debian13.deb
OpenBangla-Keyboard_2.0.0-fedora38.rpm
OpenBangla-Keyboard_2.0.0-fedora41.rpm
OpenBangla-Keyboard_2.0.0-fedora42.rpm
OpenBangla-Keyboard_2.0.0-ubuntu20.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu22.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu24.04.deb
OpenBangla-Keyboard_2.0.0-ubuntu25.04.deb"
export OB_ARCH=x86_64

pass=0; fail=0

# check <name> <expected package or "UNSUPPORTED"> <os-release content>
check() {
  local name="$1" expected="$2" out got
  printf '%s\n' "$3" > "$TMP/os-release"
  out=$(OB_OS_RELEASE="$TMP/os-release" bash "$INSTALLER" --dry-run --yes --no-setup 2>&1)
  if printf '%s\n' "$out" | grep -q '^Error:'; then
    got="UNSUPPORTED"
  else
    got=$(printf '%s\n' "$out" | sed -n 's/.*Package: *//p' | head -n1)
  fi
  if [ "$got" = "$expected" ]; then
    pass=$((pass + 1)); printf '  ok    %-28s -> %s\n' "$name" "$got"
  else
    fail=$((fail + 1)); printf '  FAIL  %-28s -> %s (expected %s)\n' "$name" "$got" "$expected"
    printf '%s\n' "$out" | sed 's/^/        | /'
  fi
}

P="OpenBangla-Keyboard_2.0.0"
echo "Distribution detection and package selection:"

check "Ubuntu 24.04"          "$P-ubuntu24.04.deb" 'ID=ubuntu
ID_LIKE=debian
VERSION_ID="24.04"
VERSION_CODENAME=noble
UBUNTU_CODENAME=noble'
check "Ubuntu 22.04"          "$P-ubuntu22.04.deb" 'ID=ubuntu
ID_LIKE=debian
VERSION_ID="22.04"'
check "Ubuntu 24.10 (between)" "$P-ubuntu24.04.deb" 'ID=ubuntu
ID_LIKE=debian
VERSION_ID="24.10"'
check "Ubuntu 26.04 (newer)"  "$P-ubuntu25.04.deb" 'ID=ubuntu
ID_LIKE=debian
VERSION_ID="26.04"'
check "Ubuntu 18.04 (too old)" "UNSUPPORTED" 'ID=ubuntu
ID_LIKE=debian
VERSION_ID="18.04"'
check "Linux Mint 22"         "$P-ubuntu24.04.deb" 'ID=linuxmint
ID_LIKE="ubuntu debian"
VERSION_ID="22"
UBUNTU_CODENAME=noble'
check "Pop!_OS 22.04"         "$P-ubuntu22.04.deb" 'ID=pop
ID_LIKE="ubuntu debian"
VERSION_ID="22.04"
UBUNTU_CODENAME=jammy'
check "Zorin OS 17"           "$P-ubuntu22.04.deb" 'ID=zorin
ID_LIKE="ubuntu debian"
VERSION_ID="17"
UBUNTU_CODENAME=jammy'
check "elementary OS 8"       "$P-ubuntu24.04.deb" 'ID=elementary
ID_LIKE=ubuntu
VERSION_ID="8"
UBUNTU_CODENAME=noble'
check "KDE neon"              "$P-ubuntu24.04.deb" 'ID=neon
ID_LIKE="ubuntu debian"
VERSION_ID="24.04"
UBUNTU_CODENAME=noble'
check "Debian 12"             "$P-debian12.deb" 'ID=debian
VERSION_ID="12"
VERSION_CODENAME=bookworm'
check "Debian 13"             "$P-debian13.deb" 'ID=debian
VERSION_ID="13"
VERSION_CODENAME=trixie'
check "Debian testing (forky)" "$P-debian13.deb" 'ID=debian
VERSION_CODENAME=forky'
check "Kali Linux Rolling"    "$P-debian13.deb" 'ID=kali
ID_LIKE=debian
VERSION_ID="2026.2"
VERSION_CODENAME=kali-rolling'
check "Parrot OS 6"           "$P-debian12.deb" 'ID=parrot
ID_LIKE=debian
VERSION_ID="6.3"
VERSION_CODENAME=lory
DEBIAN_CODENAME=bookworm'
check "MX Linux 23"           "$P-debian12.deb" 'ID=debian
VERSION_ID="12"
VERSION_CODENAME=bookworm
PRETTY_NAME="MX 23.4 Libretto"'
check "LMDE 6"                "$P-debian12.deb" 'ID=linuxmint
ID_LIKE=debian
VERSION_ID="6"
VERSION_CODENAME=bookworm'
check "Fedora 42"             "$P-fedora42.rpm" 'ID=fedora
VERSION_ID=42'
check "Fedora 44 (newer)"     "$P-fedora42.rpm" 'ID=fedora
VERSION_ID=44'
check "Fedora 40 (between)"   "$P-fedora38.rpm" 'ID=fedora
VERSION_ID=40'
check "Nobara 41"             "$P-fedora41.rpm" 'ID=nobara
ID_LIKE="rhel centos fedora"
VERSION_ID=41'
check "Arch Linux"            "$P-archlinux.pkg.tar.zst" 'ID=arch'
check "Manjaro"               "$P-archlinux.pkg.tar.zst" 'ID=manjaro
ID_LIKE=arch'
check "EndeavourOS"           "$P-archlinux.pkg.tar.zst" 'ID=endeavouros
ID_LIKE=arch'
check "openSUSE Tumbleweed"   "UNSUPPORTED" 'ID=opensuse-tumbleweed
ID_LIKE="opensuse suse"'
check "Rocky Linux 9"         "UNSUPPORTED" 'ID=rocky
ID_LIKE="rhel centos fedora"
VERSION_ID="9.5"'
check "Unknown distro"        "UNSUPPORTED" 'ID=somethingnew'

# Release 3.0.0 names packages differently (lower case, with the architecture)
# and adds ARM and openSUSE builds; see .github/workflows/deploy.yml.
V3_ASSETS="openbangla-keyboard_3.0.0-ubuntu22.04_amd64.deb
openbangla-keyboard_3.0.0-ubuntu22.04_arm64.deb
openbangla-keyboard_3.0.0-ubuntu24.04_amd64.deb
openbangla-keyboard_3.0.0-ubuntu24.04_arm64.deb
openbangla-keyboard_3.0.0-debian12_amd64.deb
openbangla-keyboard_3.0.0-debian12_arm64.deb
openbangla-keyboard_3.0.0-fedora40.x86_64.rpm
openbangla-keyboard_3.0.0-fedora41.x86_64.rpm
openbangla-keyboard_3.0.0-fedora41.aarch64.rpm
openbangla-keyboard_3.0.0-opensuse-tumbleweed.x86_64.rpm
openbangla-keyboard_3.0.0-opensuse-tumbleweed.aarch64.rpm"
V3="openbangla-keyboard_3.0.0"

v3() { OB_VERSION=3.0.0 OB_ASSETS="$V3_ASSETS" OB_ARCH="${ARCH:-x86_64}" check "$@"; }

echo
echo "Release 3.0.0 package names:"
v3 "Ubuntu 24.04"               "$V3-ubuntu24.04_amd64.deb" 'ID=ubuntu
VERSION_ID="24.04"'
ARCH=aarch64 v3 "Ubuntu 24.04 on ARM" "$V3-ubuntu24.04_arm64.deb" 'ID=ubuntu
VERSION_ID="24.04"'
v3 "Linux Mint 21"              "$V3-ubuntu22.04_amd64.deb" 'ID=linuxmint
ID_LIKE="ubuntu debian"
VERSION_ID="21.3"
UBUNTU_CODENAME=jammy'
v3 "Debian 13 (newer)"          "$V3-debian12_amd64.deb" 'ID=debian
VERSION_ID="13"'
v3 "Kali Linux Rolling"         "$V3-debian12_amd64.deb" 'ID=kali
ID_LIKE=debian
VERSION_CODENAME=kali-rolling'
v3 "Fedora 42 (newer)"          "$V3-fedora41.x86_64.rpm" 'ID=fedora
VERSION_ID=42'
ARCH=aarch64 v3 "Fedora 41 on ARM" "$V3-fedora41.aarch64.rpm" 'ID=fedora
VERSION_ID=41'
v3 "openSUSE Tumbleweed"        "$V3-opensuse-tumbleweed.x86_64.rpm" 'ID=opensuse-tumbleweed
ID_LIKE="opensuse suse"'
v3 "openSUSE Leap 15.6"         "UNSUPPORTED" 'ID=opensuse-leap
ID_LIKE="suse opensuse"
VERSION_ID="15.6"'
v3 "Arch Linux (no package)"    "UNSUPPORTED" 'ID=arch'
ARCH=riscv64 v3 "RISC-V computer" "UNSUPPORTED" 'ID=ubuntu
VERSION_ID="24.04"'

# check_im <name> <expected input method> <version> <desktop>
check_im() {
  local out got
  printf 'ID=ubuntu\nVERSION_ID="24.04"\n' > "$TMP/os-release"
  out=$(OB_OS_RELEASE="$TMP/os-release" OB_VERSION="$3" OB_DESKTOP="$4" \
        OB_ASSETS="$V3_ASSETS
$OB_ASSETS" bash "$INSTALLER" --dry-run --yes 2>&1)
  got=$(printf '%s\n' "$out" | sed -n 's/.*Input method: *\([a-z0-9]*\).*/\1/p' | head -n1)
  if [ "$got" = "$2" ]; then
    pass=$((pass + 1)); printf '  ok    %-28s -> %s\n' "$1" "$got"
  else
    fail=$((fail + 1)); printf '  FAIL  %-28s -> %s (expected %s)\n' "$1" "$got" "$2"
  fi
}

echo
echo "Input method choice:"
if pgrep -x ibus-daemon >/dev/null 2>&1 || pgrep -x fcitx5 >/dev/null 2>&1; then
  echo "  (skipped: an input method is running on this computer)"
else
  check_im "GNOME"                  ibus   3.0.0 GNOME
  check_im "KDE Plasma"             fcitx5 3.0.0 KDE
  check_im "Cinnamon"               ibus   3.0.0 X-Cinnamon
  check_im "XFCE with 3.0.0"        fcitx5 3.0.0 XFCE
  check_im "XFCE with 2.0.0"        ibus   2.0.0 XFCE
  check_im "Budgie with 2.0.0"      ibus   2.0.0 Budgie:GNOME
fi
printf 'ID=ubuntu\nVERSION_ID="24.04"\n' > "$TMP/os-release"
if OB_OS_RELEASE="$TMP/os-release" OB_VERSION=3.0.0 OB_ASSETS="$V3_ASSETS" OB_DESKTOP=GNOME \
   bash "$INSTALLER" --dry-run --yes 2>&1 | grep -q -- '--setup-system'; then
  pass=$((pass + 1)); echo "  ok    3.0.0 lets the app set up the desktop"
else
  fail=$((fail + 1)); echo "  FAIL  3.0.0 lets the app set up the desktop"
fi

echo
echo "Other checks:"
printf 'ID=ubuntu\nVERSION_ID="24.04"\n' > "$TMP/os-release"
if OB_ARCH=aarch64 OB_OS_RELEASE="$TMP/os-release" bash "$INSTALLER" --dry-run --yes 2>&1 | grep -q 'x86_64'; then
  pass=$((pass + 1)); echo "  ok    ARM computer gets a clear 'not available' message"
else
  fail=$((fail + 1)); echo "  FAIL  ARM computer message"
fi
if bash "$INSTALLER" --help | grep -q -- '--uninstall'; then
  pass=$((pass + 1)); echo "  ok    --help lists the options"
else
  fail=$((fail + 1)); echo "  FAIL  --help"
fi
if ! bash "$INSTALLER" --no-such-option >/dev/null 2>&1; then
  pass=$((pass + 1)); echo "  ok    unknown options are rejected"
else
  fail=$((fail + 1)); echo "  FAIL  unknown options are rejected"
fi

echo
echo "$pass passed, $fail failed"
[ "$fail" = 0 ]
