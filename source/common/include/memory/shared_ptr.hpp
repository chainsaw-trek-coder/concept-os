/*

    shared_ptr<T> is a poor man's version of the standard template libray's version of
    shaerd_ptr<T>.

    TODO: Make thread-safe.

          Write test cases for scenarios:
            1. Create shared_ptr with default constructor. Confirm object created and destroyed.
            2. Create shared_ptr with passed in object. Confirm object destroyed.
            3. Create shared_ptr with passed in object.
               Copy shared_ptr.
               Destroy second shared ptr.
               Confirm object not destroyed.
               Destroy first shared_ptr.
               Confirm object IS destroyed.
            4. Create shared_ptr with passed in object.
               Create copy of shared_ptr.
               Set value of initial shared_ptr.
               Confirm object not destroyed.
               Destroy both pointers.
               Confirm both objects destroyed.
            5. Create shared_ptr with passed in object.
               Set shared_ptr with new object.
               Confirm old object is destroyed.
               Destroy shared_ptr.
               Confirm new object is destroyed.

*/
namespace os::memory
{
    template <typename T>
    struct shared_object
    {
        unsigned ref_count;
        T value;
    };

    template <typename T>
    class shared_ptr
    {
    private:
        shared_object<T> *_shared;

    public:
        shared_ptr<T>();
        shared_ptr<T>(T *initial_value);
        shared_ptr<T>(shared_ptr<T> &other_ptr);

        ~shared_ptr<T>();
        operator=(T * value);

        T &value();

    private:
        void set_value(T *&initial_value);
        void decrement_reference();
    };

    template <typename T>
    inline shared_ptr<T>::shared_ptr()
    {
        _shared = new shared_object();
        _shared->ref_count = 1;
    }

    template <typename T>
    inline shared_ptr<T>::shared_ptr(T *initial_value)
    {
        if (initial_value == nullptr)
        {
            _shared = nullptr;
        }
        else
        {
            set_value(initial_value);
        }
    }

    template <typename T>
    inline shared_ptr<T>::shared_ptr(shared_ptr<T> &other_ptr)
    {
        _shared = other_ptr._shared;
        _shared->ref_count++;
    }

    template <typename T>
    inline shared_ptr<T>::~shared_ptr()
    {
        decrement_reference();
    }

    template <typename T>
    inline shared_ptr<T>::operator=(T * value)
    {
        decrement_reference(); // Already does a null check.
        set_value(value);
    }

    template <typename T>
    inline T &shared_ptr<T>::value()
    {
        // TODO: Throw exception is not initialized.
        return *_shared->value
    }

    template <typename T>
    inline void shared_ptr<T>::decrement_reference()
    {
        if (_shared != nullptr)
        {
            _shared->ref_count--;

            if (!_shared->ref_count)
            {
                delete _shared;
                _shared = nullptr; // Just in case.
            }
        }
    }

    template <typename T>
    inline void shared_ptr<T>::set_value(T *value)
    {
        if (value != nullptr)
        {
            _shared = new shared_object();
            _shared->ref_count = 1;
            _shared->value = value;
        }
        else
        {
            _shared = nullptr;
        }
    }
}