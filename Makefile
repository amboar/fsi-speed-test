all:
	arm-linux-gnueabi-gcc-6 -static -march=armv6 -O3 -o speed speed.c

.PHONY: all
