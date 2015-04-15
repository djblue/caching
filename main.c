#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"

// represents expected command line arguments
typedef struct {
  int c;        // cache size in number of pages
  char fname[1024];
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
  strcpy(in->fname, argv[2]);

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


// run trace through various algorithm
// NOTE: assume that the cache is fully-associative
//   - any item can be in any cache location
void trace (input* in) {

  trace_line l;
  int lines = 0;

  cache *lru = cache_create(in->c);
  cache *arc = cache_create(in->c);

  while (next_line(in->trace, &l)) {
    int n = l.starting_block + l.number_of_blocks;
    for (int i = l.starting_block; i < n; i++) {
      cache_lru_get(lru, i);
      cache_arc_get(arc, i);
    }
    lines++;
    if (lines % 100000 == 0) {
      fprintf(stderr, "%s(c=%d): processed %d lines\r", in->fname, in->c, lines);
      fflush(stderr);
    }
  }

  printf("file: %17s, capacity: %6d, algo: lru, ", in->fname, in->c);
  cache_print_stats(lru);
  printf("file: %17s, capacity: %6d, algo: arc, ", in->fname, in->c);
  cache_print_stats(arc);

  cache_free(lru);
  cache_free(arc);
}

int main (int argc, char **argv) {
  input in;
  if (parse_args(argc, argv, &in)) {
    return 1;
  }
  trace(&in);
  fclose(in.trace);
}
