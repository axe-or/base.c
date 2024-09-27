The files here are meant to be used as ADTs (Abstract Data Types), because C
does not have generics, building ADTs can be really annoying. So what you find
here is a compromise to use the preprocessor to instantiate generics.

All of them are "parameterized includes", that is, you `#define` some required
constants and then `#include` the file. Note that these files can be included
multiple times, they will `#undef` their arguments so you don't need to worry
about that.

The `generic.h` file just contains a couple of macro expansion trickery.

Example (Stack):
```c
```

