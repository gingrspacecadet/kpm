OUT_DIR := out
BIN_NAME := kpm
SRC_PATH := main.c
BUILD_CMD := kindle-gcc -static -o $(BIN_NAME) $(SRC_PATH)

.PHONY: all clean move local


all: $(OUT_DIR)/$(BIN_NAME)

local:
	rm -f a.out
	gcc main.c

$(OUT_DIR)/$(BIN_NAME):
	@echo "[*] Building in emulated environment..."
	$(BUILD_CMD)
	./release.sh
	clean

clean:
	rm -f $(BIN_NAME)
	$(EMULATOR) -c 'rm -f /root/$(BIN_NAME)'