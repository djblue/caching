#include <stdlib.h>

#include "map.h"

int mod(int a, int b) {
  int r = a % b;
  return r < 0 ? r + b : r;
}

map *map_create_n(int n) {
  map *m = (map*)malloc(sizeof(map));
  m->size = 0;
  m->n = n;
  m->b = (bucket**)malloc(sizeof(bucket*) * n);
  for (int i = 0; i < m->n; i++) {
    m->b[i] = NULL;
  }
  return m;
}

map *map_create() {
  return map_create_n(MAP_NUM_BUCKET);
}

void map_free(map *m) {
  for (int i = 0; i < m->n; i++) {
    bucket *temp = m->b[i], *next;
    while (temp != NULL) {
      next = temp->next;
      free(temp);
      temp = next;
    }
  }
  free(m);
}

void map_set(map *m, int key, void *value) {
  int i = mod(key, m->n);
  bucket *temp = m->b[i], *b;
  while (temp != NULL) {
    if (temp->key == key) {
      temp->value = value;
      return;
    }
    temp = temp->next;
  }
  b = (bucket*)malloc(sizeof(bucket));
  b->value = value;
  b->key = key;
  b->next = m->b[i];
  m->b[i] = b;
  m->size++;
}

void map_unset(map *m, int key) {
  int i = mod(key, m->n);
  bucket *temp = m->b[i], *prev = temp;
  while (temp != NULL) {
    if (temp->key == key) {
      if (temp == m->b[i]) {
        m->b[i] = temp->next;
      } else {
        prev->next = temp->next;
      }
      free(temp);
      m->size--;
      return;
    }
    prev = temp;
    temp = temp->next;
  }
}

void *map_get(map *m, int key) {
  int i = mod(key, m->n);
  bucket *temp = m->b[i];
  while (temp != NULL) {
    if (temp->key == key) {
      return temp->value;
    }
    temp = temp->next;
  }
  return NULL;
}

int map_has(map *m, int key) {
  int i = mod(key, m->n);
  bucket *temp = m->b[i];
  while (temp != NULL) {
    if (temp->key == key) {
      return 1; // true
    }
    temp = temp->next;
  }
  return 0; // false
}
