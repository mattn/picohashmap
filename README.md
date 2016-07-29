# picomap

FNV-1 hashmap

## Usage

```c
HMAP *m = hm_create(10, NULL);

hm_put(m, "foo", 3, "hello");
hm_put(m, "bar", 3, "world");
hm_put(m, "blah", 4, "picomap");

void *e = hm_get(m, "bar", 3); // should be "world"
```

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)
