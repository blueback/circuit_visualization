#!/bin/bash

export CFLAGS_OPTS=" -std=c++17 -O2 \
    -march=raptorlake \
    -masm=intel \
    -m64 \
    -mtune=intel"

export LDFLAGS_OPTS=" -lglfw \
    -lvulkan \
    -ldl \
    -lpthread \
    -lX11 \
    -lXxf86vm \
    -lXrandr \
    -lXi"

echo $CFLAGS_OPTS
echo $LDFLAGS_OPTS

g++ ${CFLAGS_OPTS} -o VulkanTest src/main.cpp ${OPTS}
