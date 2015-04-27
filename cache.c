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

  arc->p = 0.0;
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

void replace (cache_arc *arc, int addr) {
  page *ghost = map_get(arc->ghosts, addr);

  if (arc->t1->size != 0 &&                       // |t1| is not empty
       (arc->t1->size > arc->p ||                 // |t1| exceeds target p
         (ghost != NULL && ghost->l == arc->b2 && // x_t is in b2
            arc->t1->size == arc->p))) {          // |t1| = p
    // delete LRU in t1
    page *p = list_pop_back(arc->t1);
    if (p != NULL) {
      // remove LRU from cache
      map_unset(arc->pages, p->addr);

      // move lru into mru of b1
      list_push_front(arc->b1, p);
      map_set(arc->ghosts, p->addr, p);
    }
  } else {
    // delete LRU in t2
    page *p = list_pop_back(arc->t2);
    if (p != NULL) {
      // remove LRU from cache
      map_unset(arc->pages, p->addr);

      // move lru into mru of b2
      list_push_front(arc->b2, p);
      map_set(arc->ghosts, p->addr, p);
    }
  }
}

#define min(a,b) ((a < b)? a : b)
#define max(a,b) ((a > b)? a : b)

#define sigma1(b1,b2) ((b1->size >= b2->size)? 1 : b2->size/(double)b1->size)
#define sigma2(b1,b2) ((b2->size >= b1->size)? 1 : b1->size/(double)b2->size)

int cache_arc_get(cache_arc *arc, int addr) {

  // Case I
  // page is in cache - hit
  //  - in t1 or t2
  if (map_has(arc->pages, addr)) {

    page *p = map_get(arc->pages, addr);
    list_remove(p->l, p);
    list_push_front(arc->t2, p);

    return 1;
  }

  // Case II-III
  // page is in ghost cache
  if (map_has(arc->ghosts, addr)) {
    page *p = map_get(arc->ghosts, addr);

    // in b2
    if (p->l == arc->b1) {
      arc->p = min((arc->p + sigma1(arc->b1, arc->b2)), arc->capacity);
    // in b2
    } else {
      arc->p = max((arc->p - sigma2(arc->b1, arc->b2)), 0);
    }

    replace(arc, addr);

    // move page into lru of t2
    list_remove(p->l, p);
    list_push_front(arc->t2, p);

    // fetch page into cache
    map_unset(arc->ghosts, addr);
    map_set(arc->pages, addr, p);

    return 0;
  }

  // Case IV

  // - Case A
  if (arc->t1->size + arc->b1->size == arc->capacity) {
    if (arc->t1->size < arc->capacity) {
      page *p = list_pop_back(arc->b1);
      if (p != NULL) {
        map_unset(arc->ghosts, p->addr);
        page_free(p);
      }
      replace(arc, addr);
    } else {
      page *p = list_pop_back(arc->t1);
      if (p != NULL) {
        map_unset(arc->pages, p->addr);
        page_free(p);
      }
    }
  // - Case B
  } else {
    int len = arc->t1->size + 
              arc->t2->size +
              arc->b1->size +
              arc->b2->size;
    if (len >= arc->capacity) {
      if (len == 2*arc->capacity) {
        page *p = list_pop_back(arc->b2);
        if (p != NULL) {
          map_unset(arc->ghosts, p->addr);
          page_free(p);
        }
      }
      replace(arc, addr);
    }
  }

  page *p = page_create(addr);
  list_push_front(arc->t1, p);
  map_set(arc->pages, addr, p);

  return 0;
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

