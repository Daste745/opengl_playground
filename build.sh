#!/bin/bash -euo pipefail

NAME="main"

mkdir -p build

SOURCES="main.cpp shader_utils.cpp"
CFLAGS="$(pkg-config --cflags glfw3) -std=c++17 -Wall"
LIBS="$(pkg-config --libs --static glfw3) -framework OpenGL"
EXECUTABLE="build/$NAME"

set -x
g++ \
    $CFLAGS \
    $SOURCES \
    -o "$EXECUTABLE" \
    $LIBS

"$EXECUTABLE"
