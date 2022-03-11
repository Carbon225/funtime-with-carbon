BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out

OBJ_DIR := $(BUILD_DIR)/obj

SRC_DIR := src
INC_DIR := $(SRC_DIR)/inc

ZAD1_DIR := ../zad1
ZAD1_INC_DIR := $(ZAD1_DIR)/src/inc
ZAD1_LIB_DIR := $(ZAD1_DIR)/build/out/static

INC_DIRS := $(INC_DIR) $(ZAD1_INC_DIR)
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIRS) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS += $(INC_DIRS:%=-I%) -Wall -Werror
LDFLAGS += -L$(ZAD1_LIB_DIR) -lzad1


.PHONY: all
all: zad2

.PHONY: help
help:
	@echo './RUN [ARGS...] - compile and run'
	@echo 'make test       - run example'
	@echo 'make [all]      - compile everything'
	@echo 'make deps       - compile zad1 library'

.PHONY: test
test: zad2
	./RUN create_table 2 wc_files Makefile RUN remove_block 0 wc_files src/main.c

.PHONY: zad2
zad2:
	$(MAKE) deps
	$(MAKE) $(OUT_DIR)/zad2

.PHONY: deps
deps:
	cd $(ZAD1_DIR) && $(MAKE) static

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/zad2: $(OBJS) $(ZAD1_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^)

