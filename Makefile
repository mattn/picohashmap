prefix=/usr/local
includedir=$(prefix)/include

check: test

test: test-core
	./test-core

test-core: picomap.h test.c picotest/picotest.c picotest/picotest.h
	$(CC) -g -Wall test.c picotest/picotest.c -o $@

clean:
	rm -f test-core

install:
	install -d $(DESTDIR)$(includedir)
	install -p -m 0644 picomap.h $(DESTDIR)$(includedir)

uninstall:
	rm -f $(DESTDIR)$(includedir)/picojson.h

.PHONY: test check clean install uninstall
