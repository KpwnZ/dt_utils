SRC  := $(wildcard src/*.c)
OBJS := $(SRC:%.c=%.o)

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

all: dt_util

dt_util: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

.PHONY: install

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -m755 dt_util $(DESTDIR)$(BINDIR)
	ln -s dt_util $(DESTDIR)$(BINDIR)/dt_utils

.PHONY: clean

clean:
	rm -rf dt_util $(OBJS)
