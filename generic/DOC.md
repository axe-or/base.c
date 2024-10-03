The files here are meant to be used as ADTs (Abstract Data Types), because C
does not have generics, building ADTs can be really annoying. So what you find
here is a compromise to use the preprocessor to instantiate generics.

All of them are "parameterized includes", that is, you `#define` some required
constants and then `#include` the file. Note that these files can be included
multiple times, they will `#undef` their arguments so you don't need to worry
about that.

The `generic.h` file just contains a couple of macro expansion trickery.

## Boilerplate

```c
/* Arg check */
#if !defined(container_name) || !defined(container_type_param)
#error "Missing container_name and/or container_type_param"
#endif
#if !defined(container_prefix)
#define container_prefix M_EXP(container_name)
#endif
#define container_func(name) M_GLUE(container_prefix, _##name)
typedef struct container_name container_name;
#define generic_func static inline

/* Private macros */
#define Dyn_Array M_EXP(container_name)
#define T         M_EXP(container_type_param)


// Implementation...

#undef Dyn_Array
#undef T
#undef container_name
#undef container_type_param
#undef container_prefix
#undef container_func
#undef generic_func
```

