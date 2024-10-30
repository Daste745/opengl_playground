#!/bin/bash
set -euo pipefail

OS="$(uname)"
if [ "$OS" == "Darwin" ]; then
    CFLAGS="$(pkg-config --cflags glfw3 glew) -std=c++17 -Wall -DGL_SILENCE_DEPRECATION"
    LIBS="$(pkg-config --libs --static glfw3 glew) -framework OpenGL"
elif [ "$OS" = "Linux" ]; then
    CFLAGS="$(pkg-config --cflags glfw3 glew) -std=c++17 -Wall"
    LIBS="$(pkg-config --libs --static glfw3 glew)"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

SOURCES="$(find . -name '*.cpp')"

mkdir -p build
set -x
g++ \
    $CFLAGS \
    $SOURCES \
    -o "build/main" \
    $LIBS
