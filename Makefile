CC ?= cc
CFLAGS ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude

SRC = src/main.c src/store.c src/plan.c src/loader.c src/noise.c
BIN = hardest_c_bug

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) -o $@

asan:
	$(CC) $(CPPFLAGS) -std=c11 -O1 -g -Wall -Wextra -Wpedantic \
		-fsanitize=address,undefined -fno-omit-frame-pointer $(SRC) -o $(BIN)-asan \
		-fsanitize=address,undefined

clean:
	rm -f $(BIN) $(BIN)-asan

.PHONY: all asan clean
