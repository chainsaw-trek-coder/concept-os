#include "ipc/os_message.hpp"

os::ipc::os_message::os_message(unsigned num_of_parameters)
{
    // TODO: Request shared memory object from OS.

    // TODO: Initialize shared memory object with the number of parameters
    //       as well as an array of offsets of size equal to the number of
    //       parameters.
    
    // Copy initialization array into shared memory segment.
}

os::ipc::os_message::~os_message()
{
    // TODO: Release shared memory object back to OS.
}

unsigned os::ipc::os_message::get_value_offset(char *name)
{
    return 0;
}

void os::ipc::os_message::set_value(unsigned offset, char *value, size_t value_size)
{
}

size_t os::ipc::os_message::get_value_size(unsigned offset)
{
    return size_t();
}

void *os::ipc::os_message::get_value(unsigned offset)
{
    return nullptr;
}

void os::ipc::os_message::set_value(char *name, char *value, size_t value_size)
{
    set_value(get_value_offset(name), value, value_size);
}

size_t os::ipc::os_message::get_value_size(char *name)
{
    return get_value_size(get_value_offset(name));
}

void *os::ipc::os_message::get_value(char *name)
{
    return get_value(get_value_offset(name));
}
