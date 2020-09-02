CC ?= gcc
CFLAGS := -O -g -Wall

include mk/common.mk

EXEC = x unx

OBJS := libx.o x.o
deps := $(OBJS:%.o=%.o.d)

all: $(EXEC)

x: $(OBJS)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(OBJS) -o $@

unx: x
	$(VECHO)  "  LN\t$@\n"
	$(Q)ln -s $< $@

%.o: %.c
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

TESTDATA = 74-0.txt
74-0.txt:
	$(VECHO) "  Downloading The Adventures of Tom Sawyer by Mark Twain... "
	$(Q)wget -q https://www.gutenberg.org/files/74/74-0.txt
	$(Q)$(call pass,$@)

check: $(TESTDATA) $(EXEC)
	$(Q)./x < $< > $<.compressed
	$(Q)./unx < $<.compressed > $<.restore
	$(Q)diff $< $<.restore
	$(Q)$(RM) $<.compressed $<.restore
	$(Q)$(call pass,$(EXEC))

.PHONY: clean
clean:
	-$(RM) $(OBJS) $(EXEC) $(deps)
distclean: clean
	-$(RM) $(TESTDATA)

-include $(deps)
