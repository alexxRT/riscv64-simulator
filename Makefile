CC = clang++
INCLUDE = -I../ELFIOgit
CFLAGS = -Wno-initializer-overrides -Wno-c99-designator $(INCLUDE)
BUILDDIR = build

XCC = riscv64-elf-gcc
XCFLAGS = -nostdlib -march=rv64i

all: mkdir sample
	$(CC) $(CFLAGS) main.cpp instruction.cpp mask.cpp -o $(BUILDDIR)/simulator

sample:
	$(XCC) $(XCFLAGS) sample_rv64.s -o $(BUILDDIR)/sample_rv64

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
