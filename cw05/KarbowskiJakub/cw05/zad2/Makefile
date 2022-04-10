# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
# -----------


OBJ_DIR := $(BUILD_DIR)/obj
SRC_DIR := src
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
CFLAGS += -O$(OLEVEL) -std=gnu99


.PHONY: all
all: $(OUT_DIR)/exe

.PHONY: help
help:
	@echo './RUN [ARGS] - run program'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/exe: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
