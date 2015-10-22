PROGRAM = circle
SOURCES = circle.c statistics.c

CC       ?= gcc
CFLAGS   ?= -g -O2
CFLAGS   += -std=c99 -Wall -Wextra -pedantic
LDLIBS   += -lm

INSTALL     = install
INSTALL_BIN = $(INSTALL) -D -m 755
INSTALL_MAN = $(INSTALL) -D -m 644

PREFIX   = /usr/local

bin_dir  = $(PREFIX)/bin

all: $(PROGRAM)
.PHONY: all

$(PROGRAM): $(SOURCES)

clean:
	$(RM) *.o
.PHONY: clean

clobber: clean
	$(RM) $(PROGRAM)
.PHONY: clobber

install: $(PROGRAM)
	$(INSTALL_BIN) $(PROGRAM) $(DESTDIR)$(bin_dir)/$(PROGRAM)
.PHONY: install

install-strip:
	$(MAKE) INSTALL_BIN='$(INSTALL_BIN) -s' install
.PHONY: install-strip

uninstall:
	$(RM) $(DESTDIR)$(bin_dir)/$(PROGRAM)
.PHONY: uninstall
