#include "cache.h"

#include <stdio.h>

cache *cache_create (int capacity) {
  cache *c = malloc(sizeof(cache));

  // lru
  c->l = list_create();

  // acr
  c->l1 = list_create();
  c->l2 = list_create();
  c->ghosts = map_create_n(capacity);
  c->p = 0;

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

enum arc_position { T1, B1, T2,  B2 };

void cache_arc_replace (cache *c, page *p) {
}

void cache_arc_get(cache *c, int addr) {
  c->requests++;

  // is page already in cache
  if (map_has(c->pages, addr)) {
    page *p = map_get(c->pages, addr);
    list_remove(p->l, p);
    list_push_front(c->l2, p);
  // is in the ghost cache
  } else if (map_has(c->ghosts, addr)) {
    page *p = map_get(c->ghosts, addr);
    // in B1
    if (p->l == c->l1) {
      list_remove(p->l, p);
      list_push_front(c->l2, p);
    // in B2
    } else {
      list_remove(p->l, p);
      list_push_front(c->l2, p);
    }
    map_unset(c->ghosts, addr);
    map_set(c->pages, addr, p);
  } else {
  }

}

void cache_free (cache *c) {
  list_free(c->l);
  list_free(c->l1);
  list_free(c->l2);
  map_free(c->ghosts);
  map_free(c->pages);
  free(c);
}

void cache_print_stats(cache *c) {
  printf(
    "requests: %8d, "
    "hits: %8d, "
    "ratio: %5.2f\n",
    c->requests,
    c->hits,
    c->hits*100/(float)c->requests
  );
}
