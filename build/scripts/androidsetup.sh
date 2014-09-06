#!/bin/bash
export PKG_CONFIG_PATH=$ANDROID_PREFIX/lib/pkgconfig
export HOST=arm-linux-androideabi
export CC="ccache $HOST-gcc"
export CXX="ccache $HOST-g++"
export LD="$HOST-ld"
export AR="$HOST-ar"
export RANLIB="$HOST-ranlib"
