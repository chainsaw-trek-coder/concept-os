#include "stream.hpp"

namespace os
{
    class memory_stream : public stream
    {
        unsigned position;
        bytes data;

    public:
        memory_stream(bytes data) : position(0), data(data) {}

        bool seek(unsigned position) override;
        unsigned read_bytes(bytes destination, unsigned size) override;
        bool close() override;
    };

    inline bool memory_stream::seek(unsigned position)
    {
        this->position = position;
        return true;
    }

    inline unsigned memory_stream::read_bytes(bytes destination, unsigned size)
    {
        return destination.write_data(0, data.get_data(), position, size);
    }

    inline bool memory_stream::close()
    {
        return true;
    }
}