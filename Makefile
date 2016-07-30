prefix=/usr/local
includedir=$(prefix)/include

check: test

test: test-core
	./test-core

test-core: picohashmap.h test.c picotest/picotest.c picotest/picotest.h
	$(CC) -Wall test.c picotest/picotest.c -o $@

clean:
	rm -f test-core

benchmark:
	$(CC) -Wall benchmark.c -o benchmark
	./benchmark

install:
	install -d $(DESTDIR)$(includedir)
	install -p -m 0644 picohashmap.h $(DESTDIR)$(includedir)

uninstall:
	rm -f $(DESTDIR)$(includedir)/picojson.h

.PHONY: test check clean install uninstall
