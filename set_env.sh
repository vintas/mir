#!/bin/bash
export QUECTEL_DIR="/data/Chetan/tr069_standalone_feb24"
export TOOLCHAIN_PATH="$QUECTEL_DIR/quectel_toolchain"
export RANLIB="$TOOLCHAIN_PATH/recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-ranlib"
export LD="$TOOLCHAIN_PATH/recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-ld"
export AR="$TOOLCHAIN_PATH/recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-ar"
export SYSROOT="$TOOLCHAIN_PATH/recipe-sysroot/"
export CC="$TOOLCHAIN_PATH/recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-gcc -pthread --sysroot=$SYSROOT"
