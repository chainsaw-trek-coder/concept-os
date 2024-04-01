#include "uuid.hpp"
#include "memory/paging.hpp"

class app_instance
{
public:
    uuid id;
    page_directory* directory;
};