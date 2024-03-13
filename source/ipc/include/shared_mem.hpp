#include "guid.hpp"

struct shared_mem
{
    guid id;
    void* buffer;
    size_t size;
};