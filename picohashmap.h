#ifndef picohashmap_h
#define picohashmap_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#ifndef kcalloc
# define kcalloc(N,Z) calloc(N,Z)
#endif
#ifndef kmalloc
# define kmalloc(Z) malloc(Z)
#endif
#ifndef krealloc
# define krealloc(P,Z) realloc(P,Z)
#endif
#ifndef kfree
# define kfree(P) free(P)
#endif

#define PHM_MEM(k) k, (sizeof(k))
#define PHM_CSTR(k) k, (sizeof(k))
#define PHM_STR(k) k, (strlen(k))

#ifdef PICOHASHMAP_USE_EQUALFUNC
# define PHM_EQUALS(m,k1,k2) m->f(k1, k2)
#else
# define PHM_EQUALS(m,k1,k2) 0
#endif

typedef int (*f_phm_compare)(void*, void*);
typedef void (*f_phm_each)(void*, void*);

typedef struct _HENT HENT;

#ifdef PICOHASHMAP_USE_64BIT_HASH
typedef uint64_t phm_int_t;
#else
typedef uint32_t phm_int_t;
#endif

struct _HENT {
  void *k;
  void *v;
  phm_int_t h;
  HENT *n;
};

typedef struct {
  HENT **b;
  size_t c;
  size_t s;
  f_phm_compare f;
} PHMAP;

static inline int
bytes_eq(void* lhs, void *rhs) {
  return *(uint8_t*)lhs == *(uint8_t*)rhs;
}

static inline phm_int_t
hash(uint8_t *b, size_t l) {
  phm_int_t h;
  size_t i;
#ifdef PICOHASHMAP_USE_64BIT_HASH
  h = 14695981039346656037U;
  for (i = 0; i < l; ++i)
    h = (1099511628211LLU * h) ^ (b[i]);
#else
  h = 2166136261U;
  for (i = 0; i < l; ++i)
    h = (16777619U * h) ^ (b[i]);
#endif
  return h;
}

static inline size_t
phm_index(size_t c, phm_int_t h) {
  return ((size_t) h) & (c - 1);
}

static inline int
phm_eq(void* ka, phm_int_t ha, void* kb, phm_int_t hb) {
  if (ka == kb) return 1;
  return ha == hb;
}

int
phm_size(PHMAP *m) {
  return m->s;
}

static inline void
phm_expand(PHMAP* m) {
  if (m->s <= (m->c * 3 / 4)) return;
  size_t i;
  size_t c = m->c << 1;
  HENT** b = kcalloc(c, sizeof(HENT*));
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
  kfree(m->b);
  m->b = b;
  m->c = c;
}

#ifdef PICOHASHMAP_USE_EQUALFUNC
PHMAP*
phm_create_with_compare(size_t cap, f_phm_compare f) {
  PHMAP *m = kmalloc(sizeof(PHMAP));
  if (!m) return NULL;
  m->c = 1;
  while (m->c <= cap) m->c <<= 1;
  m->b = kcalloc(m->c, sizeof(HENT*));
  if (!m->b) {
    kfree(m);
    return NULL;
  }
  m->s = 0;
  m->f = f ? f : bytes_eq;
  return m;
}
#endif

PHMAP*
phm_create(size_t cap) {
  PHMAP *m = kmalloc(sizeof(PHMAP));
  if (!m) return NULL;
  m->c = 1;
  while (m->c <= cap) m->c <<= 1;
  m->b = kcalloc(m->c, sizeof(HENT*));
  if (!m->b) {
    kfree(m);
    return NULL;
  }
  m->s = 0;
  m->f = bytes_eq;
  return m;
}

void
phm_free(PHMAP* m) {
  size_t i;
  for (i = 0; i < m->c; i++) {
    HENT *e = m->b[i];
    while (e) {
      HENT *n = e->n;
#ifdef PICOHASHMAP_USE_COPY
      kfree(e->k);
      kfree(e->v);
#endif
      kfree(e);
      e = n;
    }
  }
  kfree(m->b);
  kfree(m);
}

static inline HENT*
phe_create(phm_int_t h, void *k, size_t ks, void *v, size_t vs) {
  HENT *e = kmalloc(sizeof(HENT));
  if (!e) return NULL;
#ifdef PICOHASHMAP_USE_COPY
  e->k = kmalloc(ks);
  if (!e->k) {
    kfree(e);
    return NULL;
  }
  memcpy(e->k, k, ks);
  e->v = kmalloc(vs);
  if (!e->v) {
    kfree(e->k);
    kfree(e->v);
    kfree(e);
    return NULL;
  }
  memcpy(e->v, v, vs);
#else
  e->k = k;
  e->v = v;
#endif
  e->h = h;
  e->n = NULL;
  return e;
}

void*
phm_put(PHMAP *m, void *k, size_t ks, void *v, size_t vs) {
  phm_int_t h = hash(k, ks);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT **p = &(m->b[i]);
  while (1) {
    HENT *cur = *p;
    if (!cur) {
      *p = phe_create(h, k, ks, v, vs);
      if (!*p) return NULL;
      m->s++;
      phm_expand(m);
      return NULL;
    }
    if (phm_eq(cur->k, cur->h, k, h) || PHM_EQUALS(m, cur->k, k)) {
      void *old = cur->v;
      cur->v = v;
      return old;
    }
    p = &cur->n;
  }
  return NULL;
}

void*
phm_get(PHMAP *m, void *k, size_t ks) {
  phm_int_t h = hash(k, ks);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT *e = m->b[i];
  while (e) {
    if (phm_eq(e->k, e->h, k, h) || PHM_EQUALS(m, e->k, k)) return e->v;
    e = e->n;
  }
  return NULL;
}

int
phm_has_key(PHMAP* m, void* k, size_t ks) {
  int h = hash(k, ks);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (phm_eq(cur->k, cur->h, k, h) || PHM_EQUALS(m, cur->k, k))
      return 1;
    p = &cur->n;
  }
  return 0;
}

void*
phm_delete(PHMAP* m, void* k, size_t ks) {
  int h = hash(k, ks);
  size_t i = ((size_t) h) & (m->c - 1);
  HENT** p = &(m->b[i]);
  HENT* cur;
  while (*p) {
    cur = *p;
    if (phm_eq(cur->k, cur->h, k, h) || PHM_EQUALS(m, cur->k, k)) {
      void* v = cur->v;
      *p = cur->n;
#ifdef PICOHASHMAP_USE_COPY
      kfree(cur->k);
      kfree(cur->v);
#endif
      kfree(cur);
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
