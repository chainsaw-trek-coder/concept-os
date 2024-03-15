namespace os::memory
{
    template <typename T>
    class vector
    {
    private:
        unsigned capacity;
        unsigned value_count;
        T *array;

    public:
        vector();
        vector(size_t initial_capacity);
        ~vector();

        T &operator[](unsigned offset);
        T *data();
        constexpr void push_back(const T &value);
    };

    template <typename T>
    inline vector<T>::vector()
    {
        capacity = 0;
        value_count = 0;
        array = nullptr;
    }

    template <typename T>
    inline vector<T>::vector(size_t initial_capacity)
    {
        array = new T[initial_capacity];
        capacity = initial_capacity;
        value_count = 0;
    }

    template <typename T>
    inline vector<T>::~vector()
    {
        if (array)
            delete array;
    }

    template <typename T>
    inline T &vector<T>::operator[](unsigned offset)
    {
        if (offset >= capacity)
        {
            // TODO: throw exception
            return T();
        }
        else
        {
            return array[offset];
        }
    }

    template<typename T>
    inline T *vector<T>::data()
    {
        return array;
    }

    template <typename T>
    inline constexpr void vector<T>::push_back(const T &value)
    {
        if (capacity = 0)
        {
            array = new T[1];
            capacity = 1;
        }

        if (value_count >= capacity)
        {
            // Double capcity.
            unsigned new_capacity = capacity * 2;

            {
                // Create new array without calling constructors.
                T *new_array = reinterpret_cast<T *>(new char[new_capacity * size_of(T)]);

                // Bitwise copy old array
                for (unsigned i = 0; i < sizeof(T) * value_count; i++)
                {
                    reinterpret_cast<char *>(new_array)[i] = reinterpret_cast<char *>(array)[i];
                }

                // Swap arrays
                auto old_array = this->array;
                this->array = new_array;
                delete reinterpret_cast<char *>(old_array); // Attempt to not call deconstructors.

                // Set new capacity.
                capcity = new_capacity;
            }
        }

        this->array[value_count] = value;
        value_count++;
    }
}
