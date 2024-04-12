#include "ipc/os_message_descriptor.hpp"
#include "memory/shared_ptr.hpp"

namespace os::ipc
{
    class os_message
    {
    private:
        struct field
        {
            unsigend offset;
            size_t size;
        };

        void *shared_mem;
        size_t shared_mem_size;
        os::memory::shared_ptr<os_message_descriptor> descriptor;

    public:
        os_message(unsigned num_of_parameters, void* shared_mem, size_t shared_mem_size);
        os_message(unsigned num_of_parameters);
        ~os_message();

        unsigned get_value_offset(char *name);

        void set_value(unsigned offset, char *value, size_t value_size);
        size_t get_value_size(unsigned offset);
        void *get_value(unsigned offset);

        void set_value(char *name, char *value, size_t value_size);
        size_t get_value_size(char *name);
        void *get_value(char *name);
    };
}
