#! /bin/bash
RELEASE_VERSION=$(cat "`dirname $0`/version.txt")
# follow cmake PACKAGE_FILE_NAME directive in main repo
RELEASE_STUB="OpenBangla-Keyboard_${RELEASE_VERSION}-"

makeDeb () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.deb"
    apt-get -y install build-essential cmake libibus-1.0-dev qt5-default rustc cargo ninja-build curl
    git clone https://github.com/OpenBangla/OpenBangla-Keyboard.git /src
    git -C /src submodule update --init --recursive
    cmake -H/src -B/build -GNinja -DCPACK_GENERATOR=DEB
    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeRpm () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.rpm"
    dnf install -y --allowerasing @buildsys-build cmake ibus-devel qt5-qtdeclarative-devel rust cargo ninja-build
    git clone https://github.com/OpenBangla/OpenBangla-Keyboard.git /src
    git -C /src submodule update --init --recursive
    cmake -H/src -B/build -GNinja -DCPACK_GENERATOR=RPM
    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeArch () {
    DIST=archlinux
    # pin package extension
    PKGEXT=".pkg.tar.xz"
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}${PKGEXT}"
    pacman -S --noconfirm --needed base-devel cmake libibus qt5-base rust curl
    mkdir /build
    sed -i "/pkgname=/a pkgver=\"${RELEASE_VERSION}\"" PKGBUILD.stub
    cp PKGBUILD.stub /build/PKGBUILD
    useradd -m builder
    chown -R builder:builder /build
    cd /build
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

if [ $DEPLOY == true ]; then
    DEPLOY_PATH="${GITHUB_WORKSPACE}/artifact"
    mkdir "$DEPLOY_PATH"
    mv "$RELEASE_FILE" "${DEPLOY_PATH}/"
fi
