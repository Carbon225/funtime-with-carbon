NAME := KarbowskiJakub

CW_DIRS := $(wildcard cw*)
TARS := $(CW_DIRS:cw%=$(NAME)-cw%.tar.gz)
ALL_FILES := $(shell find cw*)

.PHONY: all
all: $(TARS)

.PHONY: clean
clean:
	rm -f $(TARS)

$(NAME)-cw%.tar.gz: cw% $(ALL_FILES)
	cd $< && tar czf ../$@ $(NAME)

