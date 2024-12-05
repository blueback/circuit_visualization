#!/bin/bash

## export OPTS="-mmmx \
##     -msse \
##     -msse2 \
##     -msse3 \
##     -mssse3 \
##     -msse4.1 \
##     -msse4.2 \
##     -mavx \
##     -mavx2\
##     -mfma \
##     -mbmi \
##     -mbmi2 \
##     -mabm \
##     -madx \
##     -mrdrnd \
##     -march=raptorlake \
##     -masm=intel \
##     -m64 \
##     -mtune=intel"

export OPTS="-mmmx \
    -mavx2\
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

echo $OPTS

g++ tmp.cpp -S tmp.s ${OPTS}

g++ tmp.cpp -o tmp.out ${OPTS}
