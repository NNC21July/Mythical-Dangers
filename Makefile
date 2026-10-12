# Thin wrapper to preserve `mingw32-make` as the build command.
# Requires UCRT64 g++, CMake, Ninja and Git on PATH (see README).
.PHONY: all clean

all:
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
	cmake --build build --parallel 2

clean:
	cmake -E rm -rf build
