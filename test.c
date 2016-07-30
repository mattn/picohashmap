#include "picohashmap.h"
#include "picotest/picotest.h"

#include <string.h>

int
main() {
  PHMAP *m = phm_create(10, NULL);

  phm_put(m, "foo", 3, "hello");
  phm_put(m, "bar", 3, "world");
  phm_put(m, "blah", 4, "picohashmap");

#define TEST_NULL(expr) do { \
    ok(!phm_get(m, expr, strlen(expr))); \
  } while (0);

#define TEST(expr, expected) do { \
    void *e = phm_get(m, expr, strlen(expr)); \
    ok(e && !strcmp((char*)e, expected)); \
  } while (0);

  TEST("bar", "world");
  TEST("foo", "hello");
  TEST("blah", "picohashmap");
  TEST_NULL("1bar");
  TEST_NULL("fo2o");
  TEST_NULL("bla3h");

  ok(phm_has_key(m, "blah", 4));

  phm_delete(m, "blah", 4);

  ok(!phm_has_key(m, "blah", 4));

  ok(phm_size(m) == 2);

  TEST_NULL("blah");

#undef TEST
#undef TEST_NULL

  phm_free(m);

  int k;
  m = phm_create(10, NULL);

  k = 1; phm_put(m, &k, sizeof(k), "new");
  k = 2; phm_put(m, &k, sizeof(k), "york");
  k = 3; phm_put(m, &k, sizeof(k), "city");

#define TEST_NULL(expr) do { \
    k = expr; \
    ok(!phm_get(m, &k, sizeof(k))); \
  } while (0);

#define TEST(expr, expected) do { \
    k = expr; \
    void *e = phm_get(m, &k, sizeof(k)); \
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
