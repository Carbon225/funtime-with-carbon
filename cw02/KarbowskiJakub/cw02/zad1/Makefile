# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
LDFLAGS +=
# -----------

OBJ_DIR := $(BUILD_DIR)/obj

SRC_DIR := src
INC_DIR := $(SRC_DIR)/inc

INC_DIRS := $(INC_DIR)
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIRS) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wp,$(INC_DIRS:%=-I%) -O$(OLEVEL) -std=c99


.PHONY: all
all: $(OUT_DIR)/zad1

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/zad1: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
