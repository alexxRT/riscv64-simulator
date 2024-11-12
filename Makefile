CC = g++
CFLAGS =
BUILDDIR = Build

all: mkdir
	$(CC) $(CFLAGS) main.cpp functions.cpp instructions.cpp -o $(BUILDDIR)/simulator

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
