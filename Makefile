CC = clang++
INCLUDE = -I../ELFIOgit
CFLAGS = -Wno-initializer-overrides -Wno-c99-designator $(INCLUDE)
BUILDDIR = build

XCC = $(shell ./detect_xcompiler.sh)
XCFLAGS = -nostdlib -march=rv64i

all: mkdir sample
	$(CC) $(CFLAGS) main.cpp instruction.cpp mask.cpp basic_block.cpp hart.cpp -o $(BUILDDIR)/simulator

sample:
	$(XCC) $(XCFLAGS) sample_rv64.s -o $(BUILDDIR)/sample_rv64

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run:
	$(BUILDDIR)/simulator
