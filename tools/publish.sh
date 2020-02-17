#!/bin/bash
ORG='openbangla'
PACKAGE='openbangla-keyboard'
REPOLIST=(ubuntu debian fedora archlinux)
RELEASE_VERSION=$(cat version.txt | head -n1)
jfrog bt config --user "$BINTRAY_USER" --key "$BINTRAY_APIKEY" --licenses 'GPL-3.0'
pacman -Syyuu --noconfirm --needed jq rpm
cd artifact

gpgImport () {
    echo -e "-----BEGIN PGP PRIVATE KEY BLOCK-----\n\n${BINTRAY_GPG_DATA}\n-----END PGP PRIVATE KEY BLOCK-----" \
    | gpg --import 2> /dev/null
}

pubDeb () {
    # prepare an ordered list of ubuntu release codenames
    DISTS=$(curl https://api.launchpad.net/devel/ubuntu/series | jq -cM '.entries|sort_by(.version|tonumber)|map(.name)')
    for PKG in $(ls *${REPO}*); do
        # read distro code from filename
        CODENAME=$(echo $PKG | grep -oP '[^-]+.deb$' | cut -d. -f1)
        jfrog bt upload --publish --override --deb "${CODENAME}/main/amd64" "$PKG" "$VERSION_PATH"
        # we only build for 1st yearly releases. this section pushes the builds for next release version
        NEXT_CODENAME=$(echo $DISTS | jq -rcM ".[index(\"${CODENAME}\")+1]")
        if [ $NEXT_CODENAME != null ]; then
            jfrog bt upload --publish --override --deb "${NEXT_CODENAME}/main/amd64" "$PKG" "$VERSION_PATH"
        fi
    done
}

pubRpm () {
    gpgImport
    ARCH='x86_64'
    gpg --export -a "${BINTRAY_GPG_ID}" > pubkey.asc
    rpm --import pubkey.asc
    rm pubkey.asc
    echo "%_signature gpg" > "$HOME/.rpmmacros"
    echo "%_gpg_name ${BINTRAY_GPG_ID}" >> "$HOME/.rpmmacros"
    for PKG in $(ls *${REPO}*); do
        rpm --addsign "$PKG"
        VERSION_ID=$(echo $PKG | grep -oP '[\d]+.rpm$' | cut -d. -f1)
        jfrog bt upload --publish --override "$PKG" "$VERSION_PATH" "${VERSION_ID}/${ARCH}/"
    done
}

pubArch () {
    gpgImport
    ARCH='x86_64'
    # only one file should be listed anyway
    PKG=$(ls -1 *${REPO}* | head -n1)
    # get repo metadata from the "meta" version, creating it if necessary
    [[ $(jfrog bt version-show "${PACKAGE_PATH}/meta" 2> /dev/null) ]] \
    || jfrog bt version-create --vcs-tag "0.0.0" --desc "archlinux repo metadata" "${PACKAGE_PATH}/meta"
    jfrog bt download-ver "${PACKAGE_PATH}/meta"
    [ -d "$ARCH" ] && mv "$ARCH" meta || mkdir -p meta
    gpg --detach-sign --no-armor -u "$BINTRAY_GPG_ID" "$PKG"
    repo-add -p -s -k "$BINTRAY_GPG_ID" "meta/${ORG}.db.tar.gz" "$PKG"
    jfrog bt upload --publish --override "meta/*" "${PACKAGE_PATH}/meta" "${ARCH}/"
    jfrog bt upload --publish --override "${PKG}*" "${VERSION_PATH}" "${ARCH}/"
}

for REPO in ${REPOLIST[*]}; do
    PACKAGE_PATH="${ORG}/${REPO}/${PACKAGE}"
    VERSION_PATH="${PACKAGE_PATH}/${RELEASE_VERSION}"
    # create version
    [[ $(jfrog bt version-show "$VERSION_PATH" 2>/dev/null) ]] && op=version-update || op=version-create
    jfrog bt $op --vcs-tag "$RELEASE_VERSION" --released "$(date +'%Y-%m-%dT%H:%M:%S.000Z')" "$VERSION_PATH"
    # upload files
    if [[ $REPO =~ ubuntu|debian ]]; then
        pubDeb
    elif [ $REPO == fedora ]; then
        pubRpm
    elif [ $REPO == archlinux ]; then
        pubArch
    fi
done
