# picohashmap

FNV-1 hashmap

## Usage

```c
#include "picohashmap.h"
#include <string.h>

int
main() {
  PHMAP *m = phm_create(10, NULL);

  phm_put(m, "foo", 3, "hello");
  phm_put(m, "bar", 3, "world");
  phm_put(m, "blah", 4, "picohashmap");

  void *e = phm_get(m, "BAR", 3); // should be "world"
  puts((char*)e);

  phm_delete(m, "bar", 3); // return "picomap"

  if (phm_has_key(m, "BAR", 3)) {
    puts("bar is deleted");
  }
}
```

If you want to make case-insensitive keys, specify compare function into `phm_create`.

```c
#include "picohashmap.h"
#include <string.h>

int
equals(void *a, void *b) {
  return strcasecmp((char*)a, (char*)b) == 0;
}

int
main() {
  PHMAP *m = phm_create(10, equals);

  phm_put(m, "foo", 3, "hello");
  phm_put(m, "bar", 3, "world");
  phm_put(m, "blah", 4, "picohashmap");

  void *e = phm_get(m, "BAR", 3); // should be "world"
  puts((char*)e);
}
```

In default, picohashmap doesn't make a copy of keys and values. So you should case of the life time of the memory. For example, using strdup for the key and value both. If you want to use copying, use `-DPICOHASHMAP_USE_COPY`.

## Benchmark

```c
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "picohashmap.h"

int
main(int argc, char* argv[]) {
  int i;

  PHMAP *m = phm_create(10, NULL);

  time_t begin = clock();
  for (i = 0; i < 10000000u; i++) {
    phm_put(m, &i, sizeof(i), (void*)(intptr_t)i);
  }

  time_t end = clock();
  float phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("picohashmap: %f [sec]\n", phe_elapsed);
  return 0;
}
```

```
picohashmap: 3.424000 [sec]
```

Intel Core i5

With `-DPICOHASHMAP_USECOPY`

```
picohashmap: 8.936000 [sec]
```

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)
