# Makefile for the Quadruplex repo

# Compiler and linker flags
CFLAGS :=  -Wall -Werror --std=gnu99 -O3 \
	-Isrc \
	$(shell pkg-config --cflags glib-2.0 gobject-2.0 gio-2.0 gthread-2.0)
LDFLAGS := \
	-lm \
	$(shell pkg-config --libs glib-2.0 gobject-2.0 gio-2.0 gthread-2.0)
TEXFLAGS := \
	-halt-on-error \
	-file-line-error \
	-src-specials \
	-interaction=nonstopmode

ifeq ($(shell hostname),beagleboard)
	#CFLAGS += -mcpu=cortex-a8 -mfloat-abi=softfp -mfpu=neon -ftree-vectorize
	CFLAGS += -DBEAGLEBOARD=1  
endif

# Accumulator variables: BINS is a list of binaries to generate,
# CLEANS a list of files to delete on "make clean".
BINS := 
CLEANS := 

.PHONY: default clean docs report show-report install sumo

default: bin
	sync

sumo: src/sumo/sumo_demo
	sync

# The include.mk file in each directory adds things to BINS and CLEANS, as
# well as the rules necessary to build them.
MK_INCLUDES := $(shell find -name include.mk)
include $(MK_INCLUDES)

bin: $(BINS)
	@echo Updating symlinks
	@mkdir -p bin/
	@sh -c 'for f in $(BINS); do ln -sf "../$$f" bin/; done'

clean:
	rm -f $(CLEANS)
	rm -f $(BINS)
	rm -f bin/*

docs: report doc/software/figures/leg-segments.pdf

report: doc/report/report.pdf

show-report: doc/report/report.pdf
	gnome-open $<

install:
	install scripts/99-quadruplex.rules /etc/udev/rules.d/
	install scripts/quadruplex /etc/init.d/
	update-rc.d quadruplex defaults
	sync

