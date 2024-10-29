#!/bin/bash
set -euo pipefail

NAME="main"

mkdir -p build

SOURCES="$(find . -name '*.cpp')"

OS="$(uname)"
if [ "$OS" == "Darwin" ]; then
    CFLAGS="$(pkg-config --cflags glfw3) -std=c++17 -Wall -DGL_SILENCE_DEPRECATION"
    LIBS="$(pkg-config --libs --static glfw3) -framework OpenGL"
elif [ "$OS" = "Linux" ]; then
    CFLAGS="$(pkg-config --cflags glfw3 glew) -std=c++17 -Wall"
    LIBS="$(pkg-config --libs --static glfw3 glew)"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

EXECUTABLE="build/$NAME"

set -x
g++ \
    $CFLAGS \
    $SOURCES \
    -o "$EXECUTABLE" \
    $LIBS

"$EXECUTABLE"
