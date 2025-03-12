#ifndef header_527fe7a2_c795_4c0a_b3ca_93e41e5776ff
#define header_527fe7a2_c795_4c0a_b3ca_93e41e5776ff
#include "stddef.h"

// Support placement new.
inline void *operator new(size_t, void *__p) { return __p; }

#endif