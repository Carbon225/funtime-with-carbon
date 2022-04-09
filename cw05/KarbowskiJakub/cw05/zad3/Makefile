# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
# -----------


OBJ_DIR := $(BUILD_DIR)/obj

CONSUMER_SRC_DIR := src/consumer
CONSUMER_SRCS := $(shell find $(CONSUMER_SRC_DIR) -type f -and -name "*.c" -print)
CONSUMER_HDRS := $(shell find $(CONSUMER_SRC_DIR) -type f -and -name "*.h" -print)
CONSUMER_OBJS := $(CONSUMER_SRCS:%.c=$(OBJ_DIR)/%.o)

PRODUCER_SRC_DIR := src/producer
PRODUCER_SRCS := $(shell find $(PRODUCER_SRC_DIR) -type f -and -name "*.c" -print)
PRODUCER_HDRS := $(shell find $(PRODUCER_SRC_DIR) -type f -and -name "*.h" -print)
PRODUCER_OBJS := $(PRODUCER_SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS += -O$(OLEVEL) -std=gnu99


.PHONY: all
all: $(OUT_DIR)/consumer $(OUT_DIR)/producer fifo

.PHONY: help
help:
	@echo './CONSUMER [ARGS] - run consumer'
	@echo './PRODUCER [ARGS] - run producer'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR) fifo


fifo:
	mkfifo $@


$(OBJ_DIR)/%.o: %.c $(CONSUMER_HDRS) $(PRODUCER_HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<


$(OUT_DIR)/consumer: $(CONSUMER_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/producer: $(PRODUCER_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
