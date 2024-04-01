#include "memory/shared_ptr.hpp"
#include "memory/vector.hpp"

namespace os::ipc
{
    class os_message_descriptor
    {
    private:
        struct field
        {
            // TODO: Add name.
            unsigned offset;
        }

        os::memory::vector<field> parameters;

    public:
        os_message_descriptor();

        // Copy constructor.
        os_message_descriptor(os_message_descriptor &descriptor);

        void set_field(unsigned offset, const char *name);
        unsigned get_field_offset(const char *name);

        os::memory::shared_ptr<os::memory::vector<unsigned>> &get_message_initization_array();
    };
}