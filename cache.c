#include "cache.h"

#include <stdio.h>

cache_lru *cache_lru_create (int capacity) {
  cache_lru *lru = malloc(sizeof(cache_lru));

  lru->l = list_create();
  lru->pages = map_create_n(capacity);
  lru->capacity = capacity;

  return lru;
}

cache_arc *cache_arc_create (int capacity) {
  cache_arc *arc = malloc(sizeof(cache_arc));

  arc->t1 = list_create();
  arc->t2 = list_create();
  arc->b1 = list_create();
  arc->b2 = list_create();

  arc->pages = map_create_n(capacity);
  arc->ghosts = map_create_n(capacity);

  arc->p = 0;
  arc->capacity = capacity;

  return arc;
}

cache *cache_create (cache_type t, int capacity) {
  cache *c = malloc(sizeof(cache));

  c->t = t;

  switch (t) {
    case LRU:
      c->data.lru = cache_lru_create(capacity);
      break;
    case ARC:
      c->data.arc = cache_arc_create(capacity);
      break;
  }

  c->requests = 0;
  c->hits = 0;

  return c;
}


int cache_lru_get(cache_lru *lru, int addr) {

  // is page already in cache
  if (map_has(lru->pages, addr)) {

    page *p = map_get(lru->pages, addr);

    list_remove(lru->l, p);
    list_push_front(lru->l, p);

    return 1;
  }

  page *p = page_create(addr);
  map_set(lru->pages, addr, p);

  list_push_front(lru->l, p);

  // is the cache not full
  if (lru->l->size > lru->capacity) {
    page *victim = list_pop_back(lru->l);
    map_unset(lru->pages, victim->addr);
    page_free(victim);
  }

  return 0;

}


int cache_arc_get(cache_arc *arc, int addr) {
  /*
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
  */

}

void cache_get(cache *c, int addr) {
  c->requests++;
  int res = 0;
  switch (c->t) {
    case LRU:
      res = cache_lru_get(c->data.lru, addr);
      break;
    case ARC:
      res = cache_arc_get(c->data.arc, addr);
      break;
  }
  if (res == 1) {
    c->hits++;
  }
}

void cache_lru_free (cache_lru *lru) {
  list_free(lru->l);
  map_free(lru->pages);
  free(lru);
}

void cache_arc_free (cache_arc *arc) {
  list_free(arc->t1);
  list_free(arc->t2);
  list_free(arc->b1);
  list_free(arc->b2);

  map_free(arc->pages);
  map_free(arc->ghosts);

  free(arc);
}

void cache_free (cache *c) {
  switch (c->t) {
    case LRU:
      cache_lru_free(c->data.lru);
      break;
    case ARC:
      cache_arc_free(c->data.arc);
      break;
  }
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

