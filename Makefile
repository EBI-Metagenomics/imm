CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -MMD -MP
SRC = $(filter-out $(wildcard test_*.c),$(wildcard *.c))
OBJ = $(SRC:.c=.o)
HDR = $(wildcard imm_*.h)
LIB = libimm.a
PREFIX ?= /usr/local
TEST_SRC = $(wildcard test_*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = $(basename $(TEST_OBJ))

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

-include $(SRC:.c=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_TARGET): %: %.o $(LIB)
	$(CC) $(CFLAGS) $< -L. -limm -llite_pack_io -llite_pack -lm -o $@

check: $(TEST_TARGET)
	for test in $(TEST_TARGET); do ./$$test || exit 1; done

install: $(LIB) $(HDR)
	@mkdir -p $(PREFIX)/lib $(PREFIX)/include
	install -m 0755 $(LIB) $(PREFIX)/lib/
	install -m 0644 $(HDR) $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(LIB) $(HDR:%=$(PREFIX)/include/%)

.PHONY: all clean check uninstall
clean:
	rm -f $(OBJ) $(LIB) $(TEST_OBJ) $(TEST_TARGET) *.d
