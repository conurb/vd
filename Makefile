CC = clang
PACKAGES = libavformat libavutil
PKGCONFIG = pkg-config
CFLAGS = -O -Wall -Weverything
LIBES = $(shell $(PKGCONFIG) --libs $(PACKAGES))
BIN = vd

all: $(BIN)

$(BIN): $(BIN).c
	$(CC) $(CFLAGS) $(LIBES) $^ -o $@

clean:
	$(RM) -r $(BIN) *.dSYM

