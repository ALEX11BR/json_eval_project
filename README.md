# A sample C SDL2 application

This is a sample SDL2 application that can compile to many targets (web and native) and has set up some necessary things.

The `assets` folder has, well, assets; the source code sits in the root.

We have setup a build system with CMake (and optionally vcpkg) for the app's compilation:
- Compile with `cmake --preset debug ; cmake --build --preset debug` for Linux or Windows-MinGW-w64 (make sure you have installed SDL2, SDL2_image, pkg-config).
- Compile with `vcpkg install ; cmake --preset debug-vcpkg ; cmake --build --preset debug-vcpkg` for Windows-VisualStudio (probably other operating systems too, I haven't tested) with vcpkg.
- Compile with `docker run --rm -v $(pwd):/src docker.io/emscripten/emsdk bash -c 'cmake --preset emscripten ; cmake --build --preset emscripten'` for web using the emscripten docker container.

Outputs are in `out/build/<ONE_OF_THE_PRESETS>`.

We also have a `Makefile` build system for Linux, Emscripten or Windows-MinGW-w64.
