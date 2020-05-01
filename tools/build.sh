#! /bin/bash
RELEASE_VERSION=$(cat version.txt | head -n1)
# follow cmake PACKAGE_FILE_NAME directive in main repo
RELEASE_STUB="OpenBangla-Keyboard_${RELEASE_VERSION}-"

makeDeb () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.deb"
    apt-get -y install build-essential cmake libibus-1.0-dev qt5-default rustc cargo ninja-build curl
    cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCPACK_GENERATOR=DEB
    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeRpm () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.rpm"
    dnf install -y --allowerasing @buildsys-build cmake ibus-devel qt5-qtdeclarative-devel rust cargo ninja-build
    cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCPACK_GENERATOR=RPM
    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeArch () {
    DIST=archlinux
    echo "PACKAGER='OpenBangla Team <openbanglateam@gmail.com>'" >> /etc/makepkg.conf
    PKGEXT=".pkg.tar.zst"
    echo "PKGEXT='$PKGEXT'" >> /etc/makepkg.conf
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}${PKGEXT}"
    pacman -S --noconfirm --needed base-devel cmake libibus qt5-base rust curl
    mkdir /build
    cd /build
    cp -fpr "$GITHUB_WORKSPACE" /build/src
    cp "${GITHUB_WORKSPACE}/tools/PKGBUILD.stub" /build/PKGBUILD
    sed -i "/pkgname=/a pkgver=\"${RELEASE_VERSION}\"" /build/PKGBUILD
    useradd -m builder
    chown -R builder:builder /build
    sudo -u builder makepkg -fd --skipinteg
    mv openbangla-keyboard-*${PKGEXT} ${RELEASE_FILENAME}
    RELEASE_FILE=$(realpath "${RELEASE_FILENAME}")
}

if [[ $DIST =~ ^(ubuntu|debian) ]]; then
    apt-get -qq update
    apt-get -y install git
    # this is to read distro codename from filename during deployment
    CODENAME=$(cat /etc/os-release | grep "VERSION_CODENAME" | cut -d= -f2)
    DIST="${DIST}-${CODENAME}"
    BUILDFUNC=makeDeb
elif [[ $DIST =~ ^fedora ]]; then
    dnf -y --allowerasing distro-sync
    dnf -y install git
    BUILDFUNC=makeRpm
elif [[ $DIST =~ ^archlinux ]]; then
    pacman -Syyu --noconfirm --needed
    pacman -S --noconfirm --needed base git
    BUILDFUNC=makeArch
fi

DIST=${DIST/:/}
${BUILDFUNC}

DEPLOY_PATH="${GITHUB_WORKSPACE}/artifact"
mkdir "$DEPLOY_PATH"
mv "$RELEASE_FILE" "${DEPLOY_PATH}/"

