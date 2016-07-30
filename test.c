#include "picomap.h"
#include "picotest/picotest.h"

#include <string.h>

typedef struct {
  int v;
} key_int;

int
main() {
  PHMAP *m = phm_create(10, NULL);

  phm_put(m, "foo", 3, "hello");
  phm_put(m, "bar", 3, "world");
  phm_put(m, "blah", 4, "picomap");

#define TEST_NULL(expr) do { \
    ok(!phm_get(m, expr, strlen(expr))); \
  } while (0);

#define TEST(expr, expected) do { \
    void *e = phm_get(m, expr, strlen(expr)); \
    ok(e && !strcmp((char*)e, expected)); \
  } while (0);

  TEST("bar", "world");
  TEST("foo", "hello");
  TEST("blah", "picomap");
  TEST_NULL("1bar");
  TEST_NULL("fo2o");
  TEST_NULL("bla3h");

  ok(phm_has_key(m, "blah", 4));

  phm_delete(m, "blah", 4);

  ok(!phm_has_key(m, "blah", 4));

  TEST_NULL("blah");

#undef TEST
#undef TEST_NULL

  phm_free(m);

  key_int k;
  m = phm_create(10, NULL);

  k.v = 1; phm_put(m, &k.v, sizeof(k.v), "new");
  k.v = 2; phm_put(m, &k.v, sizeof(k.v), "york");
  k.v = 3; phm_put(m, &k.v, sizeof(k.v), "city");

#define TEST_NULL(expr) do { \
    key_int k = {.v = expr}; \
    ok(!phm_get(m, &k.v, sizeof(k.v))); \
  } while (0);

#define TEST(expr, expected) do { \
    key_int k = {.v = expr}; \
    void *e = phm_get(m, &k.v, sizeof(k.v)); \
    ok(e && !strcmp((char*)e, expected)); \
  } while (0);

  TEST(2, "york");
  TEST(3, "city");
  TEST(1, "new");
  TEST_NULL(4);

#undef TEST
#undef TEST_NULL

  phm_free(m);
}

/* vim:set et sw=2 cino=>2,\:0: */
