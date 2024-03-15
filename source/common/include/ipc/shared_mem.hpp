#include "guid.hpp"

namespace os::ipc
{
    struct shared_mem
    {
        guid id;
        void *buffer;
        size_t size;
    };
}