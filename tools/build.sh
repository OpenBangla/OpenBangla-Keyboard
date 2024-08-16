#! /bin/bash
RELEASE_VERSION=$(cat version.txt | head -n1)
# follow cmake PACKAGE_FILE_NAME directive in main repo
RELEASE_STUB="${IME}-openbangla_${RELEASE_VERSION}-"

makeDeb () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.deb"
    apt-get -y install build-essential pkg-config libibus-1.0-dev cmake libzstd-dev ninja-build curl qtbase5-dev qtbase5-dev-tools file
    curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal --default-toolchain stable
    
    if [[ "${IME}" == "ibus" ]]; then
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON -DCPACK_GENERATOR=DEB
    else
        apt-get -y install libfcitx5core-dev
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON -DCPACK_GENERATOR=DEB
    fi

    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeRpmFedora () {
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.rpm"
    dnf install -y --allowerasing @buildsys-build cmake ibus-devel fcitx5-devel libzstd-devel qt5-qtdeclarative-devel ninja-build curl
    curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal --default-toolchain stable
    
    if [[ "${IME}" == "ibus" ]]; then
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON -DCPACK_GENERATOR=RPM
    else
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON -DCPACK_GENERATOR=RPM
    fi

    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeRpmOpenSuse () {
    # Replace '/' with '-'
    export DIST=$(echo "$DIST" | tr '/' '-')
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}.rpm"

    zypper install -y libQt5Core-devel libQt5Widgets-devel libQt5Network-devel libzstd-devel cmake ninja ibus-devel fcitx5-devel gcc curl rpm-build
    curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal --default-toolchain stable
    
    if [[ "${IME}" == "ibus" ]]; then
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_IBUS=ON -DCPACK_GENERATOR=RPM
    else
        cmake -H"$GITHUB_WORKSPACE" -B/build -GNinja -DCMAKE_INSTALL_PREFIX="/usr" -DENABLE_FCITX=ON -DCPACK_GENERATOR=RPM
    fi

    ninja package -C /build
    RELEASE_FILE="/build/${RELEASE_FILENAME}"
}

makeArch () {
    DIST=archlinux
    echo "PACKAGER='OpenBangla Team <openbanglateam@gmail.com>'" >> /etc/makepkg.conf
    PKGEXT=".pkg.tar.zst"
    echo "PKGEXT='$PKGEXT'" >> /etc/makepkg.conf
    RELEASE_FILENAME="${RELEASE_STUB}${DIST}${PKGEXT}"
    pacman -S --noconfirm --needed base-devel cmake libibus zstd qt5-base rust curl
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
    # this is to read distro codename from filename during deployment
    #CODENAME=$(cat /etc/os-release | grep "VERSION_CODENAME" | cut -d= -f2)
    #DIST="${DIST}-${CODENAME}"
    BUILDFUNC=makeDeb
elif [[ $DIST =~ ^fedora ]]; then
    dnf -y --allowerasing distro-sync
    BUILDFUNC=makeRpmFedora
elif [[ $DIST =~ ^opensuse ]]; then
    BUILDFUNC=makeRpmOpenSuse
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

