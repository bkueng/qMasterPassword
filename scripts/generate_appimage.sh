#! /bin/bash
# Generate an AppImage (https://appimage.org/) from source

set -x
set -e

# building in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" appimage-build-XXXXXX)

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

# configure build files with cmake
cmake -B        "${BUILD_DIR}" "${REPO_ROOT}"

# build project and install files into AppDir
cmake --build   "${BUILD_DIR}" -j$(nproc)
cmake --install "${BUILD_DIR}" --prefix "${BUILD_DIR}/AppDir"

# switch to build dir
pushd "$BUILD_DIR"

# now, build AppImage using linuxdeploy and linuxdeploy-plugin-qt
# download linuxdeploy and its Qt plugin
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage

# make them executable
chmod +x linuxdeploy*.AppImage

# make sure Qt plugin finds QML sources so it can deploy the imported files
export QML_SOURCES_PATHS="$REPO_ROOT"/src

TRANSLATION_DIR="$BUILD_DIR"/AppDir/usr/share/qMasterPassword/translations
mkdir -p "$TRANSLATION_DIR"
mkdir -p "${BUILD_DIR}"/AppDir/usr/translations
cp "${BUILD_DIR}"/translations/*.qm "${TRANSLATION_DIR}"
cp "$REPO_ROOT"/data/icons/app_icon.png "$BUILD_DIR"/qmasterpassword.png

# initialize AppDir, bundle shared libraries, add desktop file and icon, use Qt plugin to bundle additional resources, and build AppImage, all in one command
./linuxdeploy-x86_64.AppImage --appdir AppDir -e qMasterPassword \
	-i "$BUILD_DIR"/qmasterpassword.png \
	-d "$REPO_ROOT"/data/qMasterPassword.desktop \
	--plugin qt --output appimage

# move built AppImage back into original CWD
mv qMasterPassword*.AppImage "$OLD_CWD"

