NAME = c97stdlib
CC = gcc
CFLAGS = -O3 -Wno-cpp -std=c11
DFLAGS = $(CFLAGS) -g
VFLAGS = $(DFLAGS) -DVERBOSE
FFLAGS = -shared -fpic
CFILES = $(wildcard ./src/[^_]*.c)
HFILES = $(wildcard ./src/[^_]*.h)
FILES = $(CFILES) $(HFILES)
BUILD_DIR = ./bld/

define BUILD_LIB
	$(CC) $(1) $(FFLAGS) $(CFILES) -o $(BUILD_DIR)$(NAME).so
	cp $(HFILES) $(BUILD_DIR).
endef

default: $(FILES)
	make clean
	$(call BUILD_LIB,$(CFLAGS))

debug: $(FILES)
	make clean
	$(call BUILD_LIB,$(DFLAGS))

verbose: $(FILES)
	make clean
	$(call BUILD_LIB,$(VFLAGS))

clean:
	rm -rf ./bld/*
