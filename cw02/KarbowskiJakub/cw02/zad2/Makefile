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

CFLAGS += -Wp,$(INC_DIRS:%=-I%) -O$(OLEVEL) -std=c99


.PHONY: all
all: $(OUT_DIR)/zad2

.PHONY: lib
lib: $(OUT_DIR)/zad2_lib

.PHONY: sys
sys: $(OUT_DIR)/zad2_sys

.PHONY: help
help:
	@echo './RUN [ARGS]     - run program'
	@echo './RUN_LIB [ARGS] - run program (lib implementation)'
	@echo './RUN_SYS [ARGS] - run program (sys implementation)'
	@echo 'make bench       - run benchmarks'

.PHONY: bench
bench: pomiar_zad_2.txt

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


pomiar_zad_2.txt: $(OUT_DIR)/zad2_sys $(OUT_DIR)/zad2_lib $(BENCH_DIR)/input
	@echo 'SYS:' > $@
	(time -p $(OUT_DIR)/zad2_sys a $(BENCH_DIR)/input) 2>> $@

	@echo 'LIB:' >> $@
	(time -p $(OUT_DIR)/zad2_lib a $(BENCH_DIR)/input) 2>> $@


$(BENCH_DIR)/input: $(shell find $(SRC_DIR) -type f) Makefile
	@mkdir -p $(dir $@)
	@rm -f $@
	@echo 'Generating input file'
	@for i in {1..10000}; do cat $^ >> $@; done


$(OBJ_DIR)/%.lib.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_LIB -c -o $@ $<

$(OBJ_DIR)/%.sys.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DIMPL_SYS -c -o $@ $<

$(OUT_DIR)/zad2_lib: $(OBJS:.o=.lib.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad2_sys: $(OBJS:.o=.sys.o)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc

$(OUT_DIR)/zad2: $(OUT_DIR)/zad2_lib
	cp $< $@
