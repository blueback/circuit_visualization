#!/bin/bash
# Modeline {
#	 vi: foldmarker={,} foldmethod=marker foldlevel=0 tabstop=4 filetype=sh
# }


THIRDPARTY_ROOT=${PWD}


# raylib {

# download raylib {
git clone https://github.com/raysan5/raylib.git
# }

# build raylib {
cd raylib/
rm -rf build
mkdir build
cd build
cmake ..
make -j28
cd ${THIRDPARTY_ROOT}
# }

# }


# libjpeg-turbo {
# NOTE: This is a dependency of turbovnc

# download libjpeg-turbo {
git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
# }

# build libjpeg-turbo {
cd libjpeg-turbo/
rm -rf build
mkdir build
cd build
cmake ..
make -j28
cd ${THIRDPARTY_ROOT}
# }

# }


# turbovnc server {

# download turbovnc server {
git clone https://github.com/TurboVNC/turbovnc.git
# }

# build turbovnc server {
cd turbovnc/
rm -rf build
mkdir build
cd build
cmake .. -DTJPEG_INCLUDE_DIR=${THIRDPARTY_ROOT}/libjpeg-turbo/src -DTJPEG_LIBRARY="-L${THIRDPARTY_ROOT}/libjpeg-turbo/build -lturbojpeg" -DTVNC_BUILDVIEWER=0
make -j28
cd ${THIRDPARTY_ROOT}
# }

# }
