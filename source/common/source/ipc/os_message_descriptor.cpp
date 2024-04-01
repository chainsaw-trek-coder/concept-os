#include "ipc/os_message_descriptor.hpp"

os::ipc::os_message_descriptor::os_message_descriptor()
{
}

os::ipc::os_message_descriptor::os_message_descriptor(os_message_descriptor &descriptor)
{
}

void os::ipc::os_message_descriptor::set_field(unsigned offset, const char *name)
{
}

unsigned os::ipc::os_message_descriptor::get_field_offset(const char *name)
{
    return -1;
}

unsigned *os::ipc::os_message_descriptor::get_message_initization_array()
{
    return nullptr;
}
