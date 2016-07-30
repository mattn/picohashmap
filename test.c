#include "picohashmap.h"
#include "picotest/picotest.h"

#include <string.h>

int
main() {
  PHMAP *m = phm_create(10, NULL);

  phm_put(m, PHM_CSTR("foo"),  PHM_CSTR("hello"));
  phm_put(m, PHM_CSTR("bar"),  PHM_CSTR("world"));
  phm_put(m, PHM_CSTR("blah"), PHM_CSTR("picohashmap"));

#define TEST_NULL(expr) do { \
    ok(!phm_get(m, expr, strlen(expr))); \
  } while (0);

#define TEST(expr, expected) do { \
    void *e = phm_get(m, PHM_CSTR(expr)); \
    ok(e && !strcmp((char*)e, expected)); \
  } while (0);

  TEST("bar", "world");
  TEST("foo", "hello");
  TEST("blah", "picohashmap");
  TEST_NULL("1bar");
  TEST_NULL("fo2o");
  TEST_NULL("bla3h");

  ok(phm_has_key(m, PHM_CSTR("blah")));

  phm_delete(m, PHM_CSTR("blah"));

  ok(!phm_has_key(m, PHM_CSTR("blah")));

  ok(phm_size(m) == 2);

  TEST_NULL("blah");

#undef TEST
#undef TEST_NULL

  phm_free(m);

  int k;
  m = phm_create(10, NULL);

  k = 1; phm_put(m, PHM_MEM(&k), PHM_CSTR("new"));
  k = 2; phm_put(m, PHM_MEM(&k), PHM_CSTR("york"));
  k = 3; phm_put(m, PHM_MEM(&k), PHM_CSTR("city"));

#define TEST_NULL(expr) do { \
    k = expr; \
    ok(!phm_get(m, PHM_MEM(&k))); \
  } while (0);

#define TEST(expr, expected) do { \
    k = expr; \
    void *e = phm_get(m, PHM_MEM(&k)); \
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
