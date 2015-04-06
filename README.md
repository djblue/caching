# caching

A project that simulates different cache replacement policies such as
least recently used (lru) and adaptive replacement cache (acr). The
simulations are done on real application caching traces.

## Building

To build the project, do:

    make

## Tests

To run all the tests, do:

    make test

## Output

A sample output file is included `output.txt`. The format is

- file: the trace file used to generate the line of output.
- capacity: the capacity of the cache in pages; ex: `32768 = (16*1024*1024)/512`.
- algo: the replacement algorithm (lru or acr).
- requests: the total number of requests to the cache.
- hits: the total number of cache hits.
- ratio: the ratio `hits/requests`.

## Details

Below are some implementation details for the cache replacement algorithms
implemented in this project.

### LRU



### ACR

TODO...

