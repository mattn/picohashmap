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

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)
