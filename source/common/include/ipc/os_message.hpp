#include <stddef.h>

namespace os::ipc
{
    class os_message
    {
    private:
        struct field
        {
            // Offset into shared memory block.
            unsigned offset;

            // Size of value.
            size_t size;
        };

        void *shared_mem;
        size_t shared_mem_size;
        unsigned *num_of_parameters_ptr;
        field *fields;
        unsigned end_of_params;
        field &get_field(unsigned offset);

    public:
        os_message(unsigned num_of_parameters, void *shared_mem, size_t shared_mem_size);
        os_message(void *shared_mem, size_t shared_mem_size);
        os_message(unsigned num_of_parameters);
        ~os_message();

        unsigned get_value_offset(char *name);

        void set_value(int offset, const char *value, size_t value_size);
        size_t get_value_size(int offset);
        void *get_value(int offset);

        void set_value(char *name, char *value, size_t value_size);
        size_t get_value_size(char *name);
        void *get_value(char *name);
    };

    inline os_message::field &os_message::get_field(unsigned offset)
    {
        return fields[offset];
    }

    inline os_message::os_message(unsigned num_of_parameters, void *shared_mem, size_t shared_mem_size)
    {
        this->shared_mem = shared_mem;
        this->shared_mem_size = shared_mem_size;
        num_of_parameters_ptr = reinterpret_cast<unsigned *>(shared_mem);
        *num_of_parameters_ptr = num_of_parameters;

        fields = reinterpret_cast<field *>(num_of_parameters_ptr + 1);

        for (unsigned i = 0; i < num_of_parameters; i++)
        {
            fields[i].offset = 0;
            fields[i].size = 0;
        }

        end_of_params = sizeof(num_of_parameters) + (sizeof(field) * num_of_parameters);
    }

    inline os_message::os_message(void *shared_mem, size_t shared_mem_size)
    {
        this->shared_mem = shared_mem;
        this->shared_mem_size = shared_mem_size;
        num_of_parameters_ptr = reinterpret_cast<unsigned *>(shared_mem);

        fields = reinterpret_cast<field *>(num_of_parameters_ptr + 1);

        end_of_params = sizeof(unsigned) + (sizeof(field) * *num_of_parameters_ptr);
    }

    inline os_message::os_message(unsigned num_of_parameters)
    {
        // TODO: Request shared memory object from OS.

        // TODO: Call this constructor
    }

    inline os_message::~os_message()
    {
        // TODO: Release shared memory object back to OS.
    }

    inline unsigned os_message::get_value_offset(char *name)
    {
        return 0;
    }

    inline void os_message::set_value(int offset, const char *value, size_t value_size)
    {
        auto &field = get_field(offset);

        // TODO: Throw an error if offset is already set?

        field.offset = end_of_params;
        field.size = value_size;

        auto dest = reinterpret_cast<char *>(shared_mem) + end_of_params;

        for (unsigned i = 0; i < value_size; i++)
            dest[i] = value[i];

        end_of_params += value_size;
    }

    inline size_t os_message::get_value_size(int offset)
    {
        return get_field(offset).size;
    }

    void *os::ipc::os_message::get_value(int offset)
    {
        return reinterpret_cast<void *>(reinterpret_cast<char *>(shared_mem) + get_field(offset).offset);
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

}