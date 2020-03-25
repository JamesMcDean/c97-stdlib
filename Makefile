NAME = c97stdlib						# Name of shared library
CC = gcc
CFLAGS = -O3 -Wno-cpp
DFLAGS = $(CFLAGS) -g
VFLAGS = $(DFLAGS) -DVERBOSE
FFLAGS = -shared -fpic
FILES = ./src/* !./src/_*.h

define BUILD_LIB
	$(CC) $(1) $(FFLAGS) -o $(NAME).so $(FILES)
	cp ./*.h ../bld/.
	rm -rf ../bld/_*.h
endef

default: $(FILES)
	make clean
	$(call BUILD_LIB,CFLAGS)

debug: $(FILES)
	make clean
	$(call BUILD_LIB,DFLAGS)

verbose: $(FILES)
	make clean
	$(call BUILD_LIB,VFLAGS)

clean:
	rm -rf ./bld/*
