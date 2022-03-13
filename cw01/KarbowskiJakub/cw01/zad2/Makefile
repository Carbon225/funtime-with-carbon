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

OLEVEL ?= 2
CFLAGS += -Wp,$(INC_DIRS:%=-I%) -Wall -Werror
LDFLAGS +=


.PHONY: all
all: zad2

.PHONY: help
help:
	@echo './RUN [ARGS...] - compile and run with ARGS'
	@echo 'make OLEVEL=x   - compile with -Ox (0, 1, 2, 3, s)'
	@echo 'make test       - run example'
	@echo 'make bench      - run benchmarks'

.PHONY: test
test: zad2
	$(OUT_DIR)/zad2 create_table 2 wc_files Makefile RUN remove_block 0 wc_files src/main.c

.PHONY: bench
bench: results3a.txt results3b.txt


.PHONY: zad2
zad2:
	$(MAKE) deps_static
	$(MAKE) $(OUT_DIR)/$@


.PHONY: zad2_static_O%
zad2_static_O%:
	$(MAKE) deps_static
	$(MAKE) $(OUT_DIR)/$@

.PHONY: zad2_shared_O%
zad2_shared_O%:
	$(MAKE) deps_shared
	$(MAKE) $(OUT_DIR)/$@

.PHONY: zad2_dll_O%
zad2_dll_O%:
	$(MAKE) deps_shared
	$(MAKE) $(OUT_DIR)/$@


.PHONY: deps_static
deps_static:
	cd $(ZAD1_DIR) && $(MAKE) static

.PHONY: deps_shared
deps_shared:
	cd $(ZAD1_DIR) && $(MAKE) shared


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


results3a.txt: $(BENCH_DIR)/static_O$(OLEVEL).txt $(BENCH_DIR)/shared_O$(OLEVEL).txt $(BENCH_DIR)/dll_O$(OLEVEL).txt
	echo 'OLEVEL=$(OLEVEL)' > $@

	echo '' >> $@
	echo 'STATIC:' >> $@
	cat $(BENCH_DIR)/static_O$(OLEVEL).txt >> $@

	echo '' >> $@
	echo 'SHARED:' >> $@
	cat $(BENCH_DIR)/shared_O$(OLEVEL).txt >> $@

	echo '' >> $@
	echo 'DLL:' >> $@
	cat $(BENCH_DIR)/dll_O$(OLEVEL).txt >> $@

results3b.txt: $(BENCH_DIR)/static_O0.txt $(BENCH_DIR)/static_O1.txt $(BENCH_DIR)/static_O2.txt $(BENCH_DIR)/static_O3.txt $(BENCH_DIR)/static_Os.txt $(BENCH_DIR)/shared_O0.txt $(BENCH_DIR)/shared_O1.txt $(BENCH_DIR)/shared_O2.txt $(BENCH_DIR)/shared_O3.txt $(BENCH_DIR)/shared_Os.txt $(BENCH_DIR)/dll_O0.txt $(BENCH_DIR)/dll_O1.txt $(BENCH_DIR)/dll_O2.txt $(BENCH_DIR)/dll_O3.txt $(BENCH_DIR)/dll_Os.txt
	echo 'STATIC -O0:' > $@
	cat $(BENCH_DIR)/static_O0.txt >> $@

	echo '' >> $@
	echo 'STATIC -O1:' >> $@
	cat $(BENCH_DIR)/static_O1.txt >> $@

	echo '' >> $@
	echo 'STATIC -O2:' >> $@
	cat $(BENCH_DIR)/static_O2.txt >> $@

	echo '' >> $@
	echo 'STATIC -O3:' >> $@
	cat $(BENCH_DIR)/static_O3.txt >> $@

	echo '' >> $@
	echo 'STATIC -Os:' >> $@
	cat $(BENCH_DIR)/static_Os.txt >> $@


	echo '' >> $@
	echo 'SHARED -O0:' >> $@
	cat $(BENCH_DIR)/shared_O0.txt >> $@

	echo '' >> $@
	echo 'SHARED -O1:' >> $@
	cat $(BENCH_DIR)/shared_O1.txt >> $@

	echo '' >> $@
	echo 'SHARED -O2:' >> $@
	cat $(BENCH_DIR)/shared_O2.txt >> $@

	echo '' >> $@
	echo 'SHARED -O3:' >> $@
	cat $(BENCH_DIR)/shared_O3.txt >> $@

	echo '' >> $@
	echo 'SHARED -Os:' >> $@
	cat $(BENCH_DIR)/shared_Os.txt >> $@


	echo '' >> $@
	echo 'DLL -O0:' >> $@
	cat $(BENCH_DIR)/dll_O0.txt >> $@

	echo '' >> $@
	echo 'DLL -O1:' >> $@
	cat $(BENCH_DIR)/dll_O1.txt >> $@

	echo '' >> $@
	echo 'DLL -O2:' >> $@
	cat $(BENCH_DIR)/dll_O2.txt >> $@

	echo '' >> $@
	echo 'DLL -O3:' >> $@
	cat $(BENCH_DIR)/dll_O3.txt >> $@

	echo '' >> $@
	echo 'DLL -Os:' >> $@
	cat $(BENCH_DIR)/dll_Os.txt >> $@


$(BENCH_DIR)/static_O%.txt: zad2_static_O%
	@mkdir -p $(dir $@)
	$(OUT_DIR)/$< bench $(shell find .. -name "*.c" -or -name "*.h") > $@

$(BENCH_DIR)/shared_O%.txt: zad2_shared_O%
	@mkdir -p $(dir $@)
	LD_LIBRARY_PATH="$(ZAD1_SHARED_LIB_DIR):${LD_LIBRARY_PATH}" $(OUT_DIR)/$< bench $(shell find .. -name "*.c" -or -name "*.h") > $@

$(BENCH_DIR)/dll_O%.txt: zad2_dll_O%
	@mkdir -p $(dir $@)
	LD_LIBRARY_PATH="$(ZAD1_SHARED_LIB_DIR):${LD_LIBRARY_PATH}" $(OUT_DIR)/$< bench $(shell find .. -name "*.c" -or -name "*.h") > $@


$(OBJ_DIR)/%.O0.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -O0 -c -o $@ $<

$(OBJ_DIR)/%.O1.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -O1 -c -o $@ $<

$(OBJ_DIR)/%.O2.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -O2 -c -o $@ $<

$(OBJ_DIR)/%.O3.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

$(OBJ_DIR)/%.Os.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Os -c -o $@ $<


$(OBJ_DIR)/%.O0.dll.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DZAD1_LIB_DLL=1,-DZAD1_LIB_PATH="\"${PWD}/$(ZAD1_SHARED_LIB_DIR)/libzad1.so\"" -O0 -c -o $@ $<

$(OBJ_DIR)/%.O1.dll.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DZAD1_LIB_DLL=1,-DZAD1_LIB_PATH="\"${PWD}/$(ZAD1_SHARED_LIB_DIR)/libzad1.so\"" -O1 -c -o $@ $<

$(OBJ_DIR)/%.O2.dll.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DZAD1_LIB_DLL=1,-DZAD1_LIB_PATH="\"${PWD}/$(ZAD1_SHARED_LIB_DIR)/libzad1.so\"" -O2 -c -o $@ $<

$(OBJ_DIR)/%.O3.dll.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DZAD1_LIB_DLL=1,-DZAD1_LIB_PATH="\"${PWD}/$(ZAD1_SHARED_LIB_DIR)/libzad1.so\"" -O3 -c -o $@ $<

$(OBJ_DIR)/%.Os.dll.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wp,-DZAD1_LIB_DLL=1,-DZAD1_LIB_PATH="\"${PWD}/$(ZAD1_SHARED_LIB_DIR)/libzad1.so\"" -Os -c -o $@ $<


$(OUT_DIR)/zad2_static_O0: $(OBJS:.o=.O0.o) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_static_O1: $(OBJS:.o=.O1.o) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_static_O2: $(OBJS:.o=.O2.o) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_static_O3: $(OBJS:.o=.O3.o) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_static_Os: $(OBJS:.o=.Os.o) $(ZAD1_STATIC_LIB_DIR)/libzad1.a
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_STATIC_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc



$(OUT_DIR)/zad2_shared_O0: $(OBJS:.o=.O0.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_shared_O1: $(OBJS:.o=.O1.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_shared_O2: $(OBJS:.o=.O2.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_shared_O3: $(OBJS:.o=.O3.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc

$(OUT_DIR)/zad2_shared_Os: $(OBJS:.o=.Os.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-L$(ZAD1_SHARED_LIB_DIR) -o $@ $(filter %.o,$^) -Wl,-lzad1,-lc


$(OUT_DIR)/zad2_dll_O0: $(OBJS:.o=.O0.dll.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,-lc

$(OUT_DIR)/zad2_dll_O1: $(OBJS:.o=.O1.dll.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,-lc

$(OUT_DIR)/zad2_dll_O2: $(OBJS:.o=.O2.dll.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,-lc

$(OUT_DIR)/zad2_dll_O3: $(OBJS:.o=.O3.dll.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,-lc

$(OUT_DIR)/zad2_dll_Os: $(OBJS:.o=.Os.dll.o) $(ZAD1_SHARED_LIB_DIR)/libzad1.so
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,-lc


$(OUT_DIR)/zad2: zad2_static_O$(OLEVEL)
	cp $(OUT_DIR)/$< $@
