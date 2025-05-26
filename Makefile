CC=gcc
CFLAGS=
BUILD_DIR=build
SRC_DIR=src

build:
	gcc $(CFLAGS) -o $(BUILD_DIR)/main main.c
