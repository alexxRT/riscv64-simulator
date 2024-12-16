CC = clang++
INCLUDE = -I../ELFIOgit
CFLAGS = -Wno-initializer-overrides -Wno-c99-designator $(INCLUDE)
BUILDDIR = build

XCC = $(shell ./detect_xcompiler.sh)
XCFLAGS = -nostdlib -march=rv64i -mabi=lp64d

all: mkdir sample 8queens
	$(CC) $(CFLAGS) main.cpp instruction.cpp mask.cpp -o $(BUILDDIR)/simulator

sample:
	$(XCC) $(XCFLAGS) sample_rv64.s -o $(BUILDDIR)/sample_rv64

8queens:
	$(XCC) $(XCFLAGS) 8queens.c -o $(BUILDDIR)/8queens

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run:
	$(BUILDDIR)/simulator
