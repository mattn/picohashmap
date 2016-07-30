# picomap

FNV-1 hashmap

## Usage

```c
PHMAP *m = phm_create(10, NULL);

phm_put(m, "foo", 3, "hello");
phm_put(m, "bar", 3, "world");
phm_put(m, "blah", 4, "picomap");

void *e = phm_get(m, "bar", 3); // should be "world"
```

## Benchmark

```c
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "picomap.h"

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
  printf("picomap: %f [sec]\n", phe_elapsed);
  return 0;
}
```

picomap: 7.779000 [sec]

Intel Core i5

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)
