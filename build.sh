#!/bin/bash
set -euo pipefail

NAME="main"

mkdir -p build

SOURCES="$(find . -name '*.cpp')"
CFLAGS="$(pkg-config --cflags glfw3) -std=c++17 -Wall -DGL_SILENCE_DEPRECATION"
LIBS="$(pkg-config --libs --static glfw3) -framework OpenGL"
EXECUTABLE="build/$NAME"

set -x
g++ \
    $CFLAGS \
    $SOURCES \
    -o "$EXECUTABLE" \
    $LIBS

"$EXECUTABLE"
