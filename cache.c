#include "cache.h"

#include <stdio.h>

cache *cache_create (int capacity) {
  cache *c = malloc(sizeof(cache));

  c->l = list_create();
  c->capacity = capacity;

  c->pages = map_create_n(capacity);
  c->requests = 0;
  c->hits = 0;

  return c;
}

void cache_lru_get(cache *c, int addr) {

  c->requests++;

  // is page already in cache
  if (map_has(c->pages, addr)) {

    page *p = map_get(c->pages, addr);

    list_remove(c->l, p);
    list_push_front(c->l, p);

    c->hits++;

  } else {

    page *p = page_create(addr);
    map_set(c->pages, addr, p);

    list_push_front(c->l, p);

    // is the cache not full
    if (c->l->size > c->capacity) {
      page *victim = list_pop_back(c->l);
      map_unset(c->pages, victim->addr);
      page_free(victim);
    }

  }

}

void cache_arc_get(cache *c, int addr) {
  c->requests++;
}

void cache_free (cache *c) {
  list_free(c->l);
  map_free(c->pages);
  free(c);
}

void cache_print_stats(cache *c) {
  printf(
    "requests: %8d, "
    "hits: %7d, "
    "ratio: %4.2f\n",
    c->requests,
    c->hits,
    c->hits*100/(float)c->requests
  );
}
