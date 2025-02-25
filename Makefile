CC = clang++
INCLUDE = -I../ELFIOgit
CFLAGS = -Wno-initializer-overrides -Wno-c99-designator -O3 $(INCLUDE) `llvm-config --cppflags --ldflags --libs` 
BUILDDIR = build
SRCDIR = .

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(SRCS:$(SRCDIR)/%.cpp=%.o))

XCC = $(shell ./detect_xcompiler.sh)
XCFLAGS = -nostdlib -march=rv64i -mabi=lp64

all: mkdir sample 8queens simulator

simulator: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILDDIR)/simulator

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

sample: sample_rv64.s
	$(XCC) $(XCFLAGS) sample_rv64.s -o $(BUILDDIR)/sample_rv64

8queens: 8queens.c
	$(XCC) $(XCFLAGS) 8queens.c -o $(BUILDDIR)/8queens

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run:
	$(BUILDDIR)/simulator
