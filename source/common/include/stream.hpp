#include "bytes.hpp"

#ifndef header_9ef0113d_dd53_4a64_a772_5834c8a034cc
#define header_9ef0113d_dd53_4a64_a772_5834c8a034cc

namespace os
{
    class stream
    {
        public:
            virtual bool seek(unsigned position) = 0;
            virtual unsigned read_bytes(bytes bytes, unsigned size) = 0;
            virtual bool close() = 0;
    };
}

#endif