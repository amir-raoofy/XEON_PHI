.PHONY: all clean

SOURCES = xtimer.c
OBJS = $(SOURCES:%.c=%.o)
COMPILE = $(CC) -c $(CPPFLAGS) $(CFLAGS)

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

all: $(OBJS)

%.o: %.c
	@echo "[CC]" $<
	@$(MKDIR_P) $(DEPDIR)
	$(COMPILE) -MD -o $@ $<
	@cp $*.d $(df).P; \
sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
rm -f $*.d

-include $(SOURCES:%.c=$(DEPDIR)/%.P)

clean:
	$(RM) -rf $(DEPDIR)
	$(RM) -f $(OBJS)
