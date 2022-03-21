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
all: $(OUT_DIR)/zad3

.PHONY: stat
stat: $(OUT_DIR)/zad3_stat

.PHONY: nftw
nftw: $(OUT_DIR)/zad3_nftw

.PHONY: help
help:
	@echo './RUN      [ARGS] - run program'
	@echo './RUN_STAT [ARGS] - run program (stat implementation)'
	@echo './RUN_NFTW [ARGS] - run program (nftw implementation)'

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.stat.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_STAT -c -o $@ $<

$(OBJ_DIR)/%.nftw.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_NFTW -c -o $@ $<

$(OUT_DIR)/zad3_stat: $(OBJS:.o=.stat.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad3_nftw: $(OBJS:.o=.nftw.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad3: $(OUT_DIR)/zad3_stat
	cp $< $@
