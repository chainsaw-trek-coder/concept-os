#include "ipc/os_message.hpp"

os::ipc::os_message::os_message(unsigned num_of_parameters, void* shared_mem, size_t shared_mem_size);
{
    auto num_of_parameters_ptr = reinterpret_cast<unsigned*>(shared_mem);
    
    *num_of_parameters_ptr = num_of_parameters;

    auto fields = reinterpret_cast<field*>(num_of_parameters_ptr + 1);

    for(unsigned i = 0; i < num_of_parameters; i++)
    {
        fields[i].offset = 0;
        fields[i].size = 0;
    }
}

os::ipc::os_message::os_message(unsigned num_of_parameters)
{
    // TODO: Request shared memory object from OS.

    // TODO: Call this constructor
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
