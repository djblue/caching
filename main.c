#include <stdio.h>
#include <stdlib.h>

#include "map.h"

// represents expected command line arguments
typedef struct {
  int c;        // cache size in number of pages
  FILE *trace;  // trace file
} input;

// represents a line in a trace file
// fields are self explanatory, see description.pdf
// for more info.
typedef struct {
  int starting_block;
  int number_of_blocks;
  int ignore;
  int request_number;
} trace_line;

// display usage info
int usage (char *bin) {
  printf(
    "USAGE: %s <c> <trace>\n"
    "  c - cache size in number of pages\n"
    "  trace - location of trace file\n"
  , bin);
  return 2;
}

// parse command line arguments
//   argc - argument count
//   argv - array of string arguments
// return 0 if successful
int parse_args (int argc, char **argv, input* in) {

  // check correct number of parameters are passed
  if (argc != 3) {
    printf("Error: not enough parameters passed\n");
    return usage(argv[0]);
  }

  // check first parameter is an integer
  if (sscanf(argv[1], "%d", &in->c) != 1) {
    printf("Error: c in not an integer\n");
    return usage(argv[0]);
  }

  // check trace file by opening it
  in->trace = fopen(argv[2], "r");
  if (in->trace == NULL) {
    printf("Error: \"%s\" is not a valid path\n", argv[2]);
    return usage(argv[0]);
  }

  // made it this far, congrats!
  return 0;
}

// parse out the next line of the trace file
//   f - open trace file pointer
//   l - line struct pointer to populate line entries
// return 0 when done
// NOTE: assuming the trace file is similar to
// the description specification
int next_line (FILE *f, trace_line *l) {

  if (fscanf(f,
    "%d %d %d %d\n", 
    &l->starting_block,
    &l->number_of_blocks,
    &l->ignore,
    &l->request_number
  ) == EOF) {
    return 0;
  }

  return 1;
}

// lru algorithm
// search for item in cache
// if item in cache
//   return item
// else
//   if cache_not_full
//     cache item
//     return item
//   else
//     replace lru_item with item

// keep map of address to cache location O(c)
//   [addr] -> cache

typedef struct page {
  struct page *next;
  struct page *prev;
  int addr;
} page;

page *page_create (int addr) {
  page *p = (page*)malloc(sizeof(page));
  p->addr = addr;
  p->next = NULL;
  p->prev = NULL;
  return p;
}

void page_free (page *p) {
  free(p);
}

typedef struct {
  page *head;
  page *tail;
  size_t size;
} list;

list *list_create () {
  list *l = (list*)malloc(sizeof(list));
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  return l;
}

void list_free (list *l) {
  page *temp = l->head, *next;

  while (temp != NULL) {
    next = temp->next;
    page_free(temp);
    temp = next;
  }

  free(l);
}

page *list_remove (list *l, page *p) {

  // remove p from list
  if (p == l->tail) {
    l->tail = p->prev;
    l->tail->next = NULL;
  } else if (p == l->head) {
    l->head = p->next;
    l->head->prev = NULL;
  } else {
    p->prev->next = p->next;
    p->next->prev = p->prev;
  }

  l->size--;

  return p;
}

void list_push_front (list *l, page *p) {

  // insert at front
  if (l->size == 0) {
    l->head = p;
    l->tail = p;
  } else {
    p->next = l->head;
    l->head->prev = p;
    l->head = p;
    p->prev = NULL;
  }

  l->size++;
}

page *list_pop_back (list *l) {
  return list_remove(l, l->tail);
}

typedef struct {

  page *head;
  page *tail;

  size_t size;
  size_t capacity;

  map *pages;
  int requests;
  int hits;

} cache;

cache *cache_create (int capacity) {
  cache *c = malloc(sizeof(cache));

  c->head = NULL;
  c->tail = NULL;

  c->size = 0;
  c->capacity = capacity;

  c->pages = map_create_n(capacity);
  c->requests = 0;
  c->hits = 0;

  return c;
}

void cache_get(cache *c, int addr) {

  c->requests++;

  // is page already in cache
  if (map_has(c->pages, addr)) {

    page *p = map_get(c->pages, addr);

    if (c->size > 1) {

      // remove p from list
      if (p == c->tail) {
        c->tail = p->prev;
        c->tail->next = NULL;
      } else if (p == c->head) {
        c->head = p->next;
        c->head->prev = NULL;
      } else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
      }

      // insert at front
      p->next = c->head;
      c->head->prev = p;
      c->head = p;
      p->prev = NULL;
    }

    c->hits++;

  } else {

    page *p = page_create(addr);
    map_set(c->pages, addr, p);

    // push element to front
    if (c->size == 0) {
      c->head = p;
      c->tail = p;
    } else {
      p->next = c->head;
      c->head->prev = p;
      c->head = p;
    }

    // is the cache not full
    if (c->size < c->capacity) {
      c->size++;

    // evict lru
    } else {
      page *victim = c->tail;
      victim->prev->next = NULL;
      c->tail = victim->prev;
      map_unset(c->pages, victim->addr);
      free(victim);
    }

  }

}

void cache_free (cache *c) {
  page *temp = c->head, *next;

  while (temp != NULL) {
    next = temp->next;
    free(temp);
    temp = next;
  }

  map_free(c->pages);
  free(c);
}

void cache_print_stats(cache *c) {
  printf(
    "== stats ===========================\n"
    "requests:  %d\n"
    "hits:      %d\n"
    "ratio:     %0.2f%\n"
    "====================================\n",
    c->requests,
    c->hits,
    c->hits*100/(float)c->requests
  );
}

// run trace through lru algorithm
// NOTE: assume that the cache is fully-associative
//   - any item can be in any cache location
void lru (input* in) {

  trace_line l;
  int lines = 0;
  cache *c = cache_create(in->c);

  while (next_line(in->trace, &l)) {
    int n = l.starting_block + l.number_of_blocks;
    for (int i = l.starting_block; i < n; i++) {
      cache_get(c, i);
    }
    lines++;
    if (lines % 100000 == 0) {
      printf("processed %d lines\n", lines);
    }
  }

  cache_print_stats(c);
  cache_free(c);
}

int main (int argc, char **argv) {
  input in;
  if (parse_args(argc, argv, &in)) {
    return 1;
  }
  lru(&in);
  fclose(in.trace);
}
