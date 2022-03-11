BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out

OBJ_DIR := $(BUILD_DIR)/obj
STATIC_DIR := $(OUT_DIR)/static
SHARED_DIR := $(OUT_DIR)/shared

SRC_DIR := src
INC_DIR := $(SRC_DIR)/inc

SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIR) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS += -I$(INC_DIR) -Wall -Werror


.PHONY: all
all: static shared

.PHONY: static
static: $(STATIC_DIR)/libzad1.a

.PHONY: shared
shared: $(SHARED_DIR)/libzad1.so

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

$(STATIC_DIR)/libzad1.a: $(OBJS)
	@mkdir -p $(dir $@)
	ar rsc $@ $^

$(SHARED_DIR)/libzad1.so: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

