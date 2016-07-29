#ifndef picomap_h
#define picomap_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

typedef int (*f_hm_compare)(void*, void*);
typedef void (*f_hm_each)(void*, void*);

typedef struct _HENT HENT;

struct _HENT {
  void *k;
  void *v;
  uint64_t h;
  HENT *n;
};

typedef struct {
  HENT **b;
  size_t c;
  size_t s;
  f_hm_compare f;
} HMAP;

static int
bytes_eq(void* lhs, void *rhs) {
  return *(uint8_t*)lhs == *(uint8_t*)rhs;
}

static uint64_t
hash(uint8_t *b, size_t l) {
  uint64_t h;
  size_t i;
  h = 14695981039346656037U;
  for (i = 0; i < l; ++i)
    h = (1099511628211LLU * h) ^ (b[i]);
  return h;
}

static size_t
hm_index(size_t c, uint64_t h) {
  return ((size_t) h) & (c - 1);
}

static int
hm_eq(void* ka, uint64_t ha, void* kb, uint64_t hb) {
  if (ka == kb) return 1;
  return ha == hb;
}

static void
hm_expand(HMAP* m) {
  if (m->s <= (m->c * 3 / 4)) return;
  size_t i;
  size_t c = m->c << 1;
  HENT** b = calloc(c, sizeof(HENT*));
  if (!b) return;
  for (i = 0; i < m->c; i++) {
    HENT* e = m->b[i];
    while (e) {
      HENT* n = e->n;
      size_t p = hm_index(c, e->h);
      e->n = b[p];
      b[p] = e;
      e = n;
    }
  }
  free(m->b);
  m->b = b;
  m->c = c;
}

HMAP*
hm_create(size_t cap, f_hm_compare f) {
  HMAP *m = malloc(sizeof(HMAP));
  if (!m) return NULL;
  m->c = 1;
  while (m->c <= cap) m->c <<= 1;
  m->b = calloc(m->c, sizeof(HENT*));
  if (!m->b) {
    free(m);
    return NULL;
  }
  m->s = 0;
  m->f = f ? f : bytes_eq;
  return m;
}

void
hm_free(HMAP* m) {
  size_t i;
  for (i = 0; i < m->c; i++) {
    HENT *e = m->b[i];
    while (e) {
      HENT *n = e->n;
      free(e->k);
      free(e);
      e = n;
    }
  }
  free(m->b);
  free(m);
}

static HENT*
he_create(void *k, size_t s, uint64_t h, void *v) {
  HENT *e = malloc(sizeof(HENT));
  if (!e) return NULL;
  e->k = malloc(s);
  if (!e->k) {
    free(e);
    return NULL;
  }
  memcpy(e->k, k, s);
  e->h = h;
  e->v = v;
  return e;
}

void*
hm_put(HMAP *m, void *k, size_t s, void *v) {
  uint64_t h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT **p = &(m->b[i]);
  while (1) {
    HENT *cur = *p;
    if (!cur) {
      *p = he_create(k, s, h, v);
      if (!*p) return NULL;
      m->s++;
      hm_expand(m);
      return NULL;
    }
    if (hm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k)) {
      void *old = cur->v;
      cur->v = v;
      return old;
    }
    p = &cur->n;
  }
  return NULL;
}

void*
hm_get(HMAP *m, void *k, size_t s) {
  uint64_t h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT *e = m->b[i];
  while (e) {
    if (hm_eq(e->k, e->h, k, h) || m->f(e->k, k)) return e->v;
    e = e->n;
  }
  return NULL;
}

int
hm_has_key(HMAP* m, void* k, size_t s) {
  int h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (hm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k))
      return 1;
    p = &cur->n;
  }
  return 0;
}

void*
hm_del(HMAP* m, void* k, size_t s) {
  int h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (hm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k)) {
      void* v = cur->v;
      *p = cur->n;
      free(cur);
      m->s--;
      return v;
    }
    p = &cur->n;
  }
  return NULL;
}

void*
hm_each(HMAP* m, f_hm_each f) {
  size_t i;
  for (i = 0; i < m->c; i++) {
    HENT *e = m->b[i];
    while (e) {
      f(e->k, e->v);
      e = e->n;
    }
  }
  return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* picomap_h */

/* vim:set et sw=2 cino=>2,\:0: */
