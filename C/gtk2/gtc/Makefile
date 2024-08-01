CFLAGS	?= -g -O0 -Wall `pkg-config --cflags gtk+-2.0`
LDFLAGS = `pkg-config --libs gtk+-2.0`
OBJS = main.o convert.o
PROG = gtc

all: gtc

gtc: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ main.c convert.c

clean:
	-rm $(PROG) >/dev/null 2>&1

check: all
	./$(PROG) | grep "Hello from .*jupiter!"
	@echo "*** All TESTS PASSED"

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 $(PROG) $(DESTDIR)$(bindir)

uninstall:
	-rm $(DESTDIR)$(bindir)/$(PROG) >/dev/null 2>&1

.PHONY: all clean check install uninstall
