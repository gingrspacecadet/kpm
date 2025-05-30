EMULATOR := ./enter-kindle.sh
OUT_DIR := out
BIN_NAME := kpm
SRC_PATH := /main.c
BUILD_CMD := gcc -static -o /root/$(BIN_NAME) $(SRC_PATH)

.PHONY: all clean move local


all: $(OUT_DIR)/$(BIN_NAME)

local:
	rm -f a.out
	gcc main.c

$(OUT_DIR)/$(BIN_NAME):
	@echo "[*] Building in emulated environment..."
	cp main.c alpine-rootfs/
	$(EMULATOR) -c '$(BUILD_CMD)'
	@mkdir -p $(OUT_DIR)
	cp alpine-rootfs/root/$(BIN_NAME) $(OUT_DIR)/
	./release.sh

clean:
	rm -f $(OUT_DIR)/$(BIN_NAME)
	$(EMULATOR) -c 'rm -f /root/$(BIN_NAME)'

move:
	cp main.c alpine-rootfs/kpm-main/
	@echo "[*] Building in emulated environment..."
	$(EMULATOR) -c 'gcc -o main kpm-main/main.c'
	$(EMULATOR)