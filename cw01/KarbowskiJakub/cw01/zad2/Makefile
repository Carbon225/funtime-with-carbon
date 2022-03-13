BUILD_DIR ?= build
OUT_DIR ?= $(BUILD_DIR)/out

BENCH_DIR := $(OUT_DIR)/bench

OBJ_DIR := $(BUILD_DIR)/obj

SRC_DIR := src
INC_DIR := $(SRC_DIR)/inc

ZAD1_DIR := ../zad1
ZAD1_INC_DIR := $(ZAD1_DIR)/src/inc
ZAD1_STATIC_LIB_DIR := $(ZAD1_DIR)/build/out/static
ZAD1_SHARED_LIB_DIR := $(ZAD1_DIR)/build/out/shared

INC_DIRS := $(INC_DIR) $(ZAD1_INC_DIR)
SRCS := $(shell find $(SRC_DIR) -type f -and -name "*.c" -print)
HDRS := $(shell find $(INC_DIRS) -type f -and -name "*.h" -print)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

OLEVEL ?= 0
CFLAGS += -Wp,$(INC_DIRS:%=-I%) -Wall -Werror -O$(OLEVEL)
LDFLAGS +=


.PHONY: all
all: zad2 zad2_static zad2_shared zad2_dll

.PHONY: help
help:
	@echo './RUN [ARGS...] - compile and run with ARGS'
	@echo 'make test       - run example'
	@echo 'make bench      - run benchmarks'

.PHONY: test
test: zad2
	$(OUT_DIR)/zad2 create_table 2 wc_files Makefile RUN remove_block 0 wc_files src/main.c

.PHONY: bench
bench: results3a.txt

.PHONY: zad2
zad2: zad2_static
	cp $(OUT_DIR)/zad2_static $(OUT_DIR)/zad2

.PHONY: zad2_static
zad2_static:
	$(MAKE) deps_static
	$(MAKE) $(OUT_DIR)/zad2_static

.PHONY: zad2_shared
zad2_shared:
	$(MAKE) deps_shared
	$(MAKE) $(OUT_DIR)/zad2_shared

.PHONY: zad2_dll
zad2_dll:
	$(MAKE) deps_shared
	$(MAKE) $(OUT_DIR)/zad2_dll

.PHONY: deps_static
deps_static:
	cd $(ZAD1_DIR) && $(MAKE) static

.PHONY: deps_shared
deps_shared:
	cd $(ZAD1_DIR) && $(MAKE) shared

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


results3a.txt: $(BENCH_DIR)/bench_static.txt $(BENCH_DIR)/bench_shared.txt $(BENCH_DIR)/bench_dll.txt
	echo 'STATIC:' > $@
	cat $(BENCH_DIR)/bench_static.txt >> $@

	echo 'SHARED:' >> $@
	cat $(BENCH_DIR)/bench_shared.txt >> $@

	echo 'DLL:' >> $@
	cat $(BENCH_DIR)/bench_dll.txt >> $@

$(BENCH_DIR)/bench_static.txt: zad2_static
	@mkdir -p $(dir $@)
	$(OUT_DIR)/zad2_static bench $(shell find .. -type f) > $@

$(BENCH_DIR)/bench_shared.txt: zad2_shared
	@mkdir -p $(dir $@)
	LD_LIBRARY_PATH="$(ZAD1_SHARED_LIB_DIR):${LD_LIBRARY_PATH}" $(OUT_DIR)/zad2_shared bench $(shell find .. -type f) > $@

$(BENCH_DIR)/bench_dll.txt: zad2_dll
	@mkdir -p $(dir $@)
	LD_LIBRARY_PATH="$(ZAD1_SHARED_LIB_DIR):${LD_LIBRARY_PATH}" $(OUT_DIR)/zad2_dll bench $(shell find .. -type f) > $@

$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/zad2_static: $(OBJS) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_shared: $(OBJS) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_dll: $(OBJS) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

