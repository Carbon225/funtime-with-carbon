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
static: static_O2

.PHONY: shared
shared: shared_O2


.PHONY: static_O0
static_O0: $(STATIC_DIR)/libzad1O0.a

.PHONY: static_O1
static_O1: $(STATIC_DIR)/libzad1O1.a

.PHONY: static_O2
static_O2: $(STATIC_DIR)/libzad1O2.a

.PHONY: static_O3
static_O3: $(STATIC_DIR)/libzad1O3.a

.PHONY: static_Os
static_Os: $(STATIC_DIR)/libzad1Os.a


.PHONY: shared_O0
shared_O0: $(SHARED_DIR)/libzad1O0.so

.PHONY: shared_O1
shared_O1: $(SHARED_DIR)/libzad1O1.so

.PHONY: shared_O2
shared_O2: $(SHARED_DIR)/libzad1O2.so

.PHONY: shared_O3
shared_O3: $(SHARED_DIR)/libzad1O3.so

.PHONY: shared_Os
shared_Os: $(SHARED_DIR)/libzad1Os.so


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)


$(OBJ_DIR)/%.O0.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -O0 -o $@ $<

$(OBJ_DIR)/%.O1.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -O1 -o $@ $<

$(OBJ_DIR)/%.O2.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -O2 -o $@ $<

$(OBJ_DIR)/%.O3.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -O3 -o $@ $<

$(OBJ_DIR)/%.Os.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -fPIC -Os -o $@ $<


$(STATIC_DIR)/libzad1O0.a: $(OBJS:.o=.O0.o)
	@mkdir -p $(dir $@)
	ar rsc $@ $^

$(STATIC_DIR)/libzad1O1.a: $(OBJS:.o=.O1.o)
	@mkdir -p $(dir $@)
	ar rsc $@ $^

$(STATIC_DIR)/libzad1O2.a: $(OBJS:.o=.O2.o)
	@mkdir -p $(dir $@)
	ar rsc $@ $^

$(STATIC_DIR)/libzad1O3.a: $(OBJS:.o=.O3.o)
	@mkdir -p $(dir $@)
	ar rsc $@ $^

$(STATIC_DIR)/libzad1Os.a: $(OBJS:.o=.Os.o)
	@mkdir -p $(dir $@)
	ar rsc $@ $^


$(SHARED_DIR)/libzad1O0.so: $(OBJS:.o=.O0.o)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

$(SHARED_DIR)/libzad1O1.so: $(OBJS:.o=.O1.o)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

$(SHARED_DIR)/libzad1O2.so: $(OBJS:.o=.O2.o)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

$(SHARED_DIR)/libzad1O3.so: $(OBJS:.o=.O3.o)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

$(SHARED_DIR)/libzad1Os.so: $(OBJS:.o=.Os.o)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^

