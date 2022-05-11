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
all: $(OUT_DIR)/master $(OUT_DIR)/cook $(OUT_DIR)/janusz

.PHONY: help
help:
	@echo './RUN [ARGS] - run pizzeria'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/master: $(filter-out $(OBJ_DIR)/src/cook.o $(OBJ_DIR)/src/janusz.o,$(OBJS))
	@mkdir -p $(dir $@)
	@echo $^
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/cook: $(filter-out $(OBJ_DIR)/src/master.o $(OBJ_DIR)/src/janusz.o,$(OBJS))
	@mkdir -p $(dir $@)
	@echo $^
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/janusz: $(filter-out $(OBJ_DIR)/src/cook.o $(OBJ_DIR)/src/master.o,$(OBJS))
	@mkdir -p $(dir $@)
	@echo $^
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
