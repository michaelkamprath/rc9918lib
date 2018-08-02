EXAMPLES = ascii sprite

INCDIR = include
SRCDIR = src
OBJDIR = obj

EXAMPLEDIR = examples
EXAMPLEBINDIR = examples/bin
LIB_SRC = $(wildcard src/*.c)

EXAMPLEAPPS = $(foreach example, $(EXAMPLES), $(EXAMPLEBINDIR)/$(addsuffix .COM,$(shell echo $(example) | tr A-Z a-z)))
EXAMPLEBINS = $(foreach example, $(EXAMPLES), $(EXAMPLEBINDIR)/$(addsuffix .bin,$(example)))

DEPS = $(wildcard include/*.h) $(wildcard src/*.h)

$(EXAMPLES): $(DEPS)
	mkdir -p $(EXAMPLEBINDIR)
	zcc +cpm -vn -I$(INCDIR) -I$(SRCDIR) $(EXAMPLEDIR)/$@.c $(LIB_SRC) -o $(EXAMPLEBINDIR)/$@.bin -create-app
	
all: $(EXAMPLES)
	rm -f $(EXAMPLEBINS)

.PHONY: clean

clean:
	rm -f $(EXAMPLEAPPS)
	rm -Rf $(EXAMPLEBINDIR)
	rm -f $(EXAMPLEDIR)/zcc_opt.def
	rm -f zcc_opt.def
