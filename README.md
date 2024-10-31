# OpenGL Playground

Playground project for experimenting and learning OpenGL in C++.

## Dependencies

- c++ compiler with version support for C++17
- cmake
- ninja

## Build & Run

```bash
# Clone with all submodules
git clone --recursive https://github.com/Daste745/opengl_playground.git opengl_playground
cd opengl_playground
# Generate CMakeFiles
cmake -B build -G Ninja .
# Build and the program
ninja -C build && build/main
build/main
```

## TODO

- [x] Use make/cmake/meson/something else for building
