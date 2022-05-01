# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
# -----------


OBJ_DIR := $(BUILD_DIR)/obj
SRC_DIR := src
INC_DIRS := $(SRC_DIR)/inc
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIRS) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
CFLAGS += -Wp,$(INC_DIRS:%=-I%) -O$(OLEVEL) -std=gnu99


.PHONY: all
all: $(OUT_DIR)/server $(OUT_DIR)/client

.PHONY: help
help:
	@echo './SERVER [ARGS] - run server'
	@echo './CLIENT [ARGS] - run client'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/server: $(filter-out $(OBJ_DIR)/src/client.o,$(OBJS))
	@mkdir -p $(dir $@)
	@echo $^
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/client: $(filter-out $(OBJ_DIR)/src/server.o,$(OBJS))
	@mkdir -p $(dir $@)
	@echo $^
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
