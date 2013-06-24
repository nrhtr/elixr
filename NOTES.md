NOTES
=====

TODOs and other things
----------------------

* Decide on garbage collection strategy (probably refcounting). This depends on how the caching setup will end up working. Simplest thing would to just do what Genesis does w/ refcounts, 'dirty' flags, etc.
* switch to libtap
* fix up various functions into methods proper
* implement networking using 0mq or crossroads io
* sort out objrefs, numrefs, etc. or equivalent.
* distinguish more clearly between internal functions and full elixr methods - reduce unecessary message sends within the codebase? not a big deal so far
* move global state into a vm struct
* implement object cache - undecided. can have enormous caches with modern memory capacities. most games would fit entirely in memory.
* switch from GNU's 0-len arrays in structs to C99 flexible array members
* implement packing/unpacking  - has dependencies on a bunch of other stuff
* implement xr type to handle files
* combine xr_tests with tests
