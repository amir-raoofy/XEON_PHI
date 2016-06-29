.PHONY: all clean

UTIL_INC_PATH = ../util
UTIL_OBJS = ../util/xtimer.o

CPPFLAGS += -I$(UTIL_INC_PATH)

PROGRAMS = false_sharing
SOURCES = false_sharing.c
OBJS = $(SOURCES:%.c=%.o)
COMPILE = $(CC) -c $(CPPFLAGS) $(CFLAGS)
LINK = $(LD) -pthread

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

all: $(PROGRAMS)

$(PROGRAMS): $(OBJS)
	@echo "[LD]" $@
	$(LINK) -o $@ $(OBJS) $(UTIL_OBJS) $(LDFLAGS)

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
	$(RM) -f $(OBJS) $(PROGRAMS)
