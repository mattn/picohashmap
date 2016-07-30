# picohashmap

[![Build Status](https://travis-ci.org/mattn/picohashmap.svg?branch=master)](https://travis-ci.org/mattn/picohashmap)

FNV-1 hashmap

## Usage

```c
#include <string.h>
#include "picohashmap.h"

int
main() {
  PHMAP *m = phm_create(10);

  phm_put(m, PHM_CSTR("foo"),  PHM_CSTR("hello"));
  phm_put(m, PHM_CSTR("bar"),  PHM_CSTR("world"));
  phm_put(m, PHM_CSTR("blah"), PHM_CSTR("picohashmap"));

  void *e = phm_get(m, PHM_CSTR("bar")); // should be "world"
  puts((char*)e);

  phm_delete(m, PHM_CSTR("bar")); // return "picohashmap"

  if (!phm_has_key(m, PHM_CSTR("bar"))) {
    puts("bar is deleted");
  }
}
```

If you want to make case-insensitive keys, specify compare function into `phm_create`.

```c
#include <string.h>
#include "picohashmap.h"

int
equals(void *a, void *b) {
  return strcasecmp((char*)a, (char*)b) == 0;
}

int
main() {
  PHMAP *m = phm_create(10, equals);

  phm_put(m, PHM_CSTR("foo"),  PHM_CSTR("hello"));
  phm_put(m, PHM_CSTR("bar"),  PHM_CSTR("world"));
  phm_put(m, PHM_CSTR("blah"), PHM_CSTR("picohashmap"));

  void *e = phm_get(m, PHM_CSTR("BAR")); // should be "world"
  puts((char*)e);
}
```

In default, picohashmap doesn't make a copy of keys and values. So you should care of a life time for the memory of the key and value. For example, using strdup for the key and value both. If you want to use copying, use `-DPICOHASHMAP_USE_COPY`.

## Benchmark

```c
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "picohashmap.h"

int
main(int argc, char* argv[]) {
  int i;

  PHMAP *m = phm_create(10);

  time_t begin = clock();
  for (i = 0; i < 10000000u; i++) {
    phm_put(m, &i, sizeof(i), &i, sizeof(i));
  }
  time_t end = clock();

  float phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("picohashmap put: %f [sec]\n", phe_elapsed);

  begin = clock();
  for (i = 0; i < 10000000u; i++) {
    phm_get(m, &i, sizeof(i));
  }
  end = clock();

  phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("picohashmap get: %f [sec]\n", phe_elapsed);
  return 0;
}
```

```
picohashmap put: 2.781000 [sec]
picohashmap get: 1.103000 [sec]
```

With `-DPICOHASHMAP_USECOPY`

```
picohashmap put: 5.950000 [sec]
picohashmap get: 1.126000 [sec]
```

Below is benchmark that works as same as above in C++ unordered_map.

```cpp
#include <iostream>
#include <string>
#include <unordered_map>
#include <ctime>

int
main(int argc, char* argv[]) {
  int i;

  std::unordered_map<int, int> m;

  time_t begin = clock();
  for (i = 0; i < 10000000u; i++) {
    m[i] = i;
  }
  time_t end = clock();

  float phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("C++ stl put: %f [sec]\n", phe_elapsed);

  begin = clock();
  for (i = 0; i < 10000000u; i++) {
    int v = m[i];
  }
  end = clock();

  phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("C++ stl get: %f [sec]\n", phe_elapsed);
  return 0;
}
```

```
C++ stl put: 5.126669 [sec]
C++ stl get: 1.272198 [sec]
```

And klib khash

```c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "khash.h"

KHASH_MAP_INIT_INT(v, int);

int
main(int argc, char* argv[]) {
  int r, i;
  khash_t(v)* h = kh_init(v);

  time_t begin = clock();
  for (i = 0; i < 10000000u; i++) {
    khint_t k = kh_put(v, h, i, &r);
    kh_value(h, k) = i;
  }
  time_t end = clock();
  float phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("klib hash put: %f [sec]\n", phe_elapsed);

  begin = clock();
  for (i = 0; i < 10000000u; i++) {
    khint_t k = kh_get(v, h, i);
    if (k != kh_end(h))
      kh_value(h, k);
    else
      abort();
  }
  end = clock();
  phe_elapsed = (float)(end - begin) / CLOCKS_PER_SEC;
  printf("klib hash get: %f [sec]\n", phe_elapsed);

  return 0;
}
```

Well, khash is too fast. It's a monster.

```
klib hash put: 0.252785 [sec]
klib hash get: 0.036985 [sec]
```

Intel Core i5

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)
