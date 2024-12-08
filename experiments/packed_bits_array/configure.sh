#!/usr/bin/bash
rm -rf build

export GCC_11_PATH="/usr/lib/"
export GCC_11_PATH="${GCC_11_PATH}:/usr/local/lib/"
export LD_LIBRARY_PATH="${GCC_11_PATH}:${LD_LIBRARY_PATH}"

export GCC_11="/usr/local/bin/gcc"
export GPP_11="/usr/local/bin/g++"

cmake -H. -Bbuild \
	-DCMAKE_C_COMPILER=${GCC_11} \
	-DCMAKE_CXX_COMPILER=${GPP_11} \
	-DCMAKE_BUILD_TYPE=Debug

make -Cbuild -j28

export ASM_OPTS="-mmmx \
    -O3 \
    -mavx2 \
    -mavxvnni\
    -mfma \
    -mbmi \
    -mbmi2 \
    -mabm \
    -madx \
    -mrdrnd \
    -march=raptorlake \
    -masm=intel \
    -m64 \
    -mtune=intel"

echo $ASM_OPTS

g++ src/tmp.cpp -S tmp.s ${ASM_OPTS}
