# USER CONFIG
BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out
OLEVEL ?= 2
CFLAGS += -Wall -Werror
LDFLAGS +=
# -----------

OBJ_DIR := $(BUILD_DIR)/obj
BENCH_DIR := $(BUILD_DIR)/bench

SRC_DIR := src
INC_DIR := $(SRC_DIR)/inc

INC_DIRS := $(INC_DIR)
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIRS) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wp,$(INC_DIRS:%=-I%) -O$(OLEVEL) -std=gnu99


.PHONY: all
all: $(OUT_DIR)/zad1

.PHONY: lib
lib: $(OUT_DIR)/zad1_lib

.PHONY: sys
sys: $(OUT_DIR)/zad1_sys

.PHONY: help
help:
	@echo './RUN [ARGS]     - run program'
	@echo './RUN_LIB [ARGS] - run program (lib implementation)'
	@echo './RUN_SYS [ARGS] - run program (sys implementation)'
	@echo 'make bench       - run benchmarks'

.PHONY: bench
bench: pomiar_zad_1.txt

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


pomiar_zad_1.txt: $(OUT_DIR)/zad1_sys $(OUT_DIR)/zad1_lib $(BENCH_DIR)/input comments.txt
	@echo 'SYS:' > $@
	(time -p $(OUT_DIR)/zad1_sys $(BENCH_DIR)/input $(BENCH_DIR)/output) 2>> $@

	@echo 'LIB:' >> $@
	(time -p $(OUT_DIR)/zad1_lib $(BENCH_DIR)/input $(BENCH_DIR)/output) 2>> $@

	@echo '' >> $@
	@cat comments.txt >> $@


$(BENCH_DIR)/input: $(shell find $(SRC_DIR) -type f) Makefile
	@mkdir -p $(dir $@)
	@rm -f $@
	@echo 'Generating input file'
	@for i in {1..5000}; do cat $^ >> $@; done


$(OBJ_DIR)/%.lib.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_LIB -c -o $@ $<

$(OBJ_DIR)/%.sys.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_SYS -c -o $@ $<

$(OUT_DIR)/zad1_lib: $(OBJS:.o=.lib.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad1_sys: $(OBJS:.o=.sys.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad1: $(OUT_DIR)/zad1_lib
	cp $< $@
