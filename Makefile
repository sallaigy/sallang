CC   = /usr/bin/gcc
YACC = /usr/bin/bison
LEX  = /usr/bin/flex

CFLAGS  = -Wall -g -O0 -Isrc
LDFLAGS = -lm

## SLGVM ##

SLGVM_HEADERS=src/vm/slgvm.h src/vm/vm_opcodes.h src/vm/slgasm.h src/vm/slgdisasm.h
SLGVM_SOURCES=src/vm/slgvm.c src/slgvm_main.c src/vm/slgasm.c src/vm/slgdisasm.c

SLGVM_HEADERS+=src/common/hash.h src/common/list.h src/common/bstrlib.h
SLGVM_SOURCES+=src/common/hash.c src/common/list.c src/common/bstrlib.c

SLGVM_OBJECTS=$(patsubst src/%.c,obj/%.o,$(SLGVM_SOURCES))

SLGVM_TARGET=slgvm

## SLGC ##

SLGC_HEADERS  = src/parser/ast.h src/parser/compile.h src/parser/irlang.h src/parser/symbol.h
SLGC_HEADERS += src/slg.h src/common/hash.h src/common/list.h src/common/vector.h src/parser/snode.h
SLGC_HEADERS += src/parser/types.h src/parser/error.h
SLGC_HEADERS += src/common/bstrlib.h

SLGC_GENERATED_HEADERS = src/parser/parser.h
SLGC_GENERATED_SOURCES = src/parser/parser.c src/parser/scanner.c

SLGC_SOURCES  = src/parser/ast.c src/parser/compile.c src/slgc_main.c src/parser/symbol.c src/parser/snode.c
SLGC_SOURCES += src/common/hash.c src/common/list.c src/common/vector.c src/common/bstrlib.c
SLGC_SOURCES += src/parser/types.c src/parser/error.c
SLGC_SOURCES += $(SLGC_GENERATED_SOURCES)

SLGC_OBJECTS=$(patsubst src/%.c,obj/%.o,$(SLGC_SOURCES))

SLGC_TARGET=slgc

## Tests ##

TEST_SOURCES=tests/vm/slgasm_tests.c

TESTS=$(patsubst %.c,%,$(TEST_SOURCES))

all: $(SLGVM_TARGET) $(SLGC_TARGET)

$(SLGVM_TARGET): $(SLGVM_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(SLGC_TARGET): $(SLGC_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c  $(SLGC_GENERATED_SOURCES)
	$(CC) -c $(CFLAGS) -o $@ $<

## SLGC specific ##
src/parser/scanner.c: src/parser/scanner.l src/parser/parser.c src/parser/parser.h
	$(LEX) -o $@ $<

src/parser/parser.c src/parser/parser.h: src/parser/parser.y
	$(YACC) --verbose -d $< -o $@

## Tests ##

.PHONY: tests
tests: $(TESTS)
	sh ./tests/runtests.sh

tests/%: tests/%_tests.c tests/vm/minunit.h
	$(CC) $(CFLAGS) -o $@ $^

tests/vm/slgasm_tests: tests/vm/slgasm_tests.c tests/vm/minunit.h
	$(CC) $(CFLAGS) -o $@ $<

## Cleaning ##

clean:
	@rm $(SLGVM_OBJECTS)
	@rm $(SLGC_GENERATED_SOURCES)
	@rm $(SLGC_GENERATED_HEADERS)
	@rm $(SLGC_OBJECTS)
	@rm $(SLGC_TARGET) $(SLGVM_TARGET)
