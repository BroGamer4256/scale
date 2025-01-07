all:
	@meson compile -C build
	@strip build/scale.dll

setup:
	@meson setup build --cross cross-mingw-64.txt
