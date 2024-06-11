#include "bytes.hpp"

namespace os
{
    class stream
    {
        public:
            virtual bool seek(unsigned position);
            virtual unsigned read_bytes(bytes &bytes, unsigned size);
            virtual bool close();
    }
}