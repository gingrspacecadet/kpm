EMULATOR := ./enter-kindle.sh
OUT_DIR := out
BIN_NAME := kpm
SRC_PATH := /main.c
BUILD_CMD := gcc -static -o /root/$(BIN_NAME) $(SRC_PATH)

.PHONY: all clean move local


all: $(OUT_DIR)/$(BIN_NAME)

local:
	rm -f a.out
	mkdir -p local
	touch local/kpm.conf
	echo \# Example config file > local/kpm.conf
	echo \# Installation directory >> local/kpm.conf
	echo INSTALL_DIR=local/packages >> local/kpm.conf
	echo \# Mirror config file >> local/kpm.conf
	echo MIRRORS_CONF=local/kpm_mirrors.conf >> local/kpm.conf
	echo \# Package cache directory >> local/kpm.conf
	echo TMP_LIST_FILE=local/kpm_packages.list >> local/kpm.conf
	touch local/kpm_mirrors.conf
	echo https://raw.githubusercontent.com/gingrspacecadet/kpm/refs/heads/main/packages.list https://github.com/gingrspacecadet/kpm/releases/download/{pkg}/{pkg}.zip > local/kpm_mirrors.conf
	gcc main.c -DLOCAL

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