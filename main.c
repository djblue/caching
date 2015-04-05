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

// run trace through lru algorithm
// NOTE: assume that the cache is fully-associative
//   - any item can be in any cache location
void lru (input* in) {
  trace_line l;
  int hit = 0, requests = 0;
  while (next_line(in->trace, &l)) {
    
    requests += l.number_of_blocks;
  }
  printf("requests: %d\n", requests);
}

int main (int argc, char **argv) {
  input in;
  if (parse_args(argc, argv, &in)) {
    return 1;
  }
  lru(&in);
  fclose(in.trace);
}
