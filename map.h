#ifndef MAP_H
#define MAP_H

// default number of buckets
#define MAP_NUM_BUCKET 1024 

typedef struct bucket {
  int key;
  void *value;
  struct bucket *next;
} bucket;

typedef struct {
  bucket **b;  // array of bucket pointers
  size_t n;    // number of buckets
  size_t size; // how many element are currently stored
} map;

map *map_create();
map *map_create_n(int n);
void map_free(map *m);

void map_set(map *m, int key, void *value);
void map_unset(map *m, int key);

void *map_get(map *m, int key);
int map_has(map *m, int key);

#endif
