#ifndef picohashmap_h
#define picohashmap_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

typedef int (*f_phm_compare)(void*, void*);
typedef void (*f_phm_each)(void*, void*);

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
  f_phm_compare f;
} PHMAP;

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
phm_index(size_t c, uint64_t h) {
  return ((size_t) h) & (c - 1);
}

static int
phm_eq(void* ka, uint64_t ha, void* kb, uint64_t hb) {
  if (ka == kb) return 1;
  return ha == hb;
}

int
phm_size(PHMAP *m) {
  return m->s;
}

static void
phm_expand(PHMAP* m) {
  if (m->s <= (m->c * 3 / 4)) return;
  size_t i;
  size_t c = m->c << 1;
  HENT** b = calloc(c, sizeof(HENT*));
  if (!b) return;
  for (i = 0; i < m->c; i++) {
    HENT* e = m->b[i];
    while (e) {
      HENT* n = e->n;
      size_t p = phm_index(c, e->h);
      e->n = b[p];
      b[p] = e;
      e = n;
    }
  }
  free(m->b);
  m->b = b;
  m->c = c;
}

PHMAP*
phm_create(size_t cap, f_phm_compare f) {
  PHMAP *m = malloc(sizeof(PHMAP));
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
phm_free(PHMAP* m) {
  size_t i;
  for (i = 0; i < m->c; i++) {
    HENT *e = m->b[i];
    while (e) {
      HENT *n = e->n;
#ifdef PICOHASHMAP_COPYKEYS
      free(e->k);
#endif
      free(e);
      e = n;
    }
  }
  free(m->b);
  free(m);
}

static HENT*
phe_create(void *k, size_t s, uint64_t h, void *v) {
  HENT *e = malloc(sizeof(HENT));
  if (!e) return NULL;
#ifdef PICOHASHMAP_COPYKEYS
  e->k = malloc(s);
  if (!e->k) {
    free(e);
    return NULL;
  }
  memcpy(e->k, k, s);
#else
  e->k = k;
#endif
  e->h = h;
  e->v = v;
  e->n = NULL;
  return e;
}

void*
phm_put(PHMAP *m, void *k, size_t s, void *v) {
  uint64_t h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT **p = &(m->b[i]);
  while (1) {
    HENT *cur = *p;
    if (!cur) {
      *p = phe_create(k, s, h, v);
      if (!*p) return NULL;
      m->s++;
      phm_expand(m);
      return NULL;
    }
    if (phm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k)) {
      void *old = cur->v;
      cur->v = v;
      return old;
    }
    p = &cur->n;
  }
  return NULL;
}

void*
phm_get(PHMAP *m, void *k, size_t s) {
  uint64_t h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT *e = m->b[i];
  while (e) {
    if (phm_eq(e->k, e->h, k, h) || m->f(e->k, k)) return e->v;
    e = e->n;
  }
  return NULL;
}

int
phm_has_key(PHMAP* m, void* k, size_t s) {
  int h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (phm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k))
      return 1;
    p = &cur->n;
  }
  return 0;
}

void*
phm_delete(PHMAP* m, void* k, size_t s) {
  int h = hash(k, s);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (phm_eq(cur->k, cur->h, k, h) || m->f(cur->k, k)) {
      void* v = cur->v;
      *p = cur->n;
#ifdef PICOHASHMAP_COPYKEYS
      free(e->k);
#endif
      free(cur);
      m->s--;
      return v;
    }
    p = &cur->n;
  }
  return NULL;
}

void*
phm_each(PHMAP* m, f_phm_each f) {
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

#endif /* picohashmap_h */

/* vim:set et sw=2 cino=>2,\:0: */
