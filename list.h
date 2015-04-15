#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct list;

typedef struct page {
  struct page *next;
  struct page *prev;
  struct list *l;
  int addr;
} page;

page *page_create (int addr);
void page_free (page *p);

typedef struct list {
  page *head;
  page *tail;
  size_t size;
} list;

list *list_create ();
void list_free (list *l);
page *list_remove (list *l, page *p);
page *list_pop_back (list *l);
void list_push_front (list *l, page *p);

#endif
