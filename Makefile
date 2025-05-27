CC=gcc
CFLAGS=
BUILD_DIR=build
SRC_DIR=src

build:
	mkdir -p build
	gcc $(CFLAGS) -o $(BUILD_DIR)/kpm main.c
	mv $(BUILD_DIR)/kpm /usr/bin
