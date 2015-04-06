#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>

#include "map.h"
#include "list.h"

typedef struct {

  list *l;
  size_t capacity;

  map *pages;
  int requests;
  int hits;

} cache;

cache *cache_create (int capacity);
void cache_free (cache *c);

void cache_lru_get(cache *c, int addr);
void cache_arc_get(cache *c, int addr);
void cache_print_stats(cache *c);

#endif
