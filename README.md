
## Toys

Just thinking of ways to insert pre/post condition type stuff into code
after the fact. You'd like to have them in prior to implementation, but
say you want or have to add such annotations on a code base that's already
there. One way to think of going about that process is to look at varying
levels of where functions are defined, depth-wise, by how many unique functions
they call.
So, looking at functions that have 0 call instructions, 1 call instructions, 
..., >k call instructions lumped. This may or may not be useful for you
depending on goals.

Aside from the above, these are mostly just for-fun, so not likely worth
your while, but putting here so they are available. They modify the 
module instead of trying to create a new module with the stuff in it.

### Couple of issues

- Not yet handling other references to functions (eg, pthread_create(.,.,&function,.))
- Probably just awful in general. Could benefit from lambda
