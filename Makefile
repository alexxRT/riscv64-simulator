CC = clang++
INCLUDE = -I../ELFIOgit
CFLAGS = -Wno-initializer-overrides -Wno-c99-designator -O3 $(INCLUDE)
BUILDDIR = build

XCC = $(shell ./detect_xcompiler.sh)
XCFLAGS = -nostdlib -march=rv64i -mabi=lp64

all: mkdir sample
	$(CC) $(CFLAGS) main.cpp instruction.cpp mask.cpp basic_block.cpp hart.cpp -o $(BUILDDIR)/simulator

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
