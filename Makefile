
.PHONY: all clean

all:
	cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
	cmake --build build --parallel 2

clean:
	cmake -E rm -rf build
