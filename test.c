#include "picomap.h"
#include "picotest/picotest.h"

#include <string.h>

typedef struct {
  int v;
} key_int;

int
main() {
  HMAP *m = hm_create(10, NULL);

  hm_put(m, "foo", 3, "hello");
  hm_put(m, "bar", 3, "world");
  hm_put(m, "blah", 4, "picomap");

#define TEST_NULL(expr) do { \
    ok(!hm_get(m, expr, strlen(expr))); \
  } while (0);

#define TEST(expr, expected) do { \
    void *e = hm_get(m, expr, strlen(expr)); \
    ok(!strcmp((char*)e, expected)); \
  } while (0);

  TEST("bar", "world");
  TEST("foo", "hello");
  TEST("blah", "picomap");
  TEST_NULL("1bar");
  TEST_NULL("fo2o");
  TEST_NULL("bla3h");

#undef TEST
#undef TEST_NULL

  hm_free(m);

  key_int k;
  m = hm_create(10, NULL);

  k.v = 1; hm_put(m, &k.v, sizeof(k.v), "new");
  k.v = 2; hm_put(m, &k.v, sizeof(k.v), "york");
  k.v = 3; hm_put(m, &k.v, sizeof(k.v), "city");

#define TEST_NULL(expr) do { \
    key_int k = {(int)expr}; \
    ok(!hm_get(m, &k.v, sizeof(expr))); \
  } while (0);

#define TEST(expr, expected) do { \
    key_int k = {(int)expr}; \
    void *e = hm_get(m, &k.v, sizeof(k.v)); \
    ok(!strcmp((char*)e, expected)); \
  } while (0);

  TEST(2, "york");
  TEST(3, "city");
  TEST(1, "new");
  TEST_NULL((intptr_t)"1bar");
  TEST_NULL(3);
  TEST_NULL('2');

#undef TEST
#undef TEST_NULL

  hm_free(m);
}

/* vim:set et sw=2 cino=>2,\:0: */
