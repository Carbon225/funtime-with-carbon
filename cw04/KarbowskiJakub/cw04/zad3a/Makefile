# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
# -----------


OBJ_DIR := $(BUILD_DIR)/obj
SRC_DIR := src
CFLAGS += -O$(OLEVEL) -std=gnu99


.PHONY: all
all: $(OUT_DIR)/sender $(OUT_DIR)/catcher

.PHONY: help
help:
	@echo './CATCHER [ARGS] - run catcher'
	@echo './SENDER  [ARGS] - run sender'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OUT_DIR)/%: $(OBJ_DIR)/%.o
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
