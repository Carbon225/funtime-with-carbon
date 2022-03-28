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
all: $(OUT_DIR)/exe

.PHONY: help
help:
	@echo './RUN [ARGS] - run program'
	@echo 'make bench   - run benchmarks'

.PHONY: bench
bench: results.txt

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


results.txt: $(OUT_DIR)/exe
	./bench.sh $< > $@
	@echo '' >> $@
	@cat comments.txt >> $@


$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/exe: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ -Wl,-lc
