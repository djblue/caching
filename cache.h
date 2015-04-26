#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>

#include "map.h"
#include "list.h"

typedef enum { LRU, ARC } cache_type;

typedef struct {
  list *l;
  map *pages;
  size_t capacity;
} cache_lru;

typedef struct {
  list *t1, *t2, *b1, *b2;
  map *ghosts;
  map *pages;
  int p;
  size_t capacity;
} cache_arc;

typedef struct {

  cache_type t;

  union {
    cache_lru *lru;
    cache_arc *arc;
  } data;

  // for lru
  /*
  list *l;
  map *pages;
  */

  // for acr
  /*
  list *t1, *t2, *b1, *b2;
  map *ghosts;
  int p;
  */

  unsigned long requests;
  unsigned long hits;

} cache;

cache *cache_create (cache_type t, int capacity);
void cache_free (cache *c);

void cache_get(cache *c, int addr);
void cache_print_stats(cache *c);

#endif
