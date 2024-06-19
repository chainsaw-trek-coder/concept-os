#ifndef header_30e2b229_767c_427f_8b1a_ad72965d55f4
#define header_30e2b229_767c_427f_8b1a_ad72965d55f4

namespace os
{
    class bytes
    {
    private:
        void *data;
        unsigned size;

    public:
        bytes(void *data, unsigned size) : data(data), size(size) {}

        unsigned get_size() { return size; }
        void *get_data() { return data; }
        unsigned write_data(unsigned destination_position, void *source, unsigned source_position, unsigned source_size);
    };

    inline unsigned bytes::write_data(unsigned destination_position, void *source, unsigned source_position, unsigned source_size)
    {
        unsigned original_source_position = source_position;
        
        while(source_size > 0)
        {
            if(destination_position >= size)
                break;

            if(source_position >= source_size)
                break;

            reinterpret_cast<unsigned char*>(data)[destination_position] = reinterpret_cast<unsigned char*>(source)[source_position];

            destination_position++;
            source_position++;
        }

        return source_position - original_source_position;        
    }

}

#endif