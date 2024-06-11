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
        void write_data(unsigned destination_position, void *source, unsigned source_position, unsigned source_size);
    };

    inline bytes::write_data(unsigned destination_position, void *source, unsigned source_position, unsigned source_size)
    {
        while(source_size > 0)
        {
            if(destination_position >= size)
                break;

            if(source_position >= source_size)
                break;

            reinterpret_cast<unsigned char*>(data)[destination_position] = reinterpret_cast<unsigned char*>(source)[source_position];

            destination_position++;
            source_position++;
            source_size--;
        }
        
    }

}

#endif