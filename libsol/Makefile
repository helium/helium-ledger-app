mode = debug
target := $(shell uname -s)
variant = $(target)_$(mode)
o = target/$(variant)

test_files := $(wildcard *_test.c)
test_exes = $(patsubst %.c,$o/%,$(test_files))
test_oks = $(addsuffix .ok,$(test_exes))

all: $(test_oks) $(test_exes) $o/libsol.a

CFLAGS += -Werror -Wall -Wextra -pedantic -Wshadow -Wcast-qual -Wcast-align -Wno-unused-parameter
CFLAGS += -fPIC
CFLAGS += $($(mode)_CFLAGS)

debug_CFLAGS = -g
release_CFLAGS = -O2

$o/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

#
# unit tests
#
# Note: this executes on the host, not via QEMU
$o/%_test.ok: $o/%_test
	@echo "Testing $<..."
	@$< && touch $@

$o/%_test: %_test.o $o/libsol.a
	$(CC) $(CFLAGS) -o $@ $^

#
# libsol
#
libsol_source_files = $(filter-out %_test.c,$(wildcard *.c))
libsol_object_files = $(patsubst %.c,$o/%.o,$(libsol_source_files))
$o/libsol.a: $(libsol_object_files)
	ar rvs $@ $^

#
# clean
#
.PHONY: clean
clean:
	rm -rf target
