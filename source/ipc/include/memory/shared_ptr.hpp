
/*

    shared_ptr<T> is a poor man's version of the standard template libray's version of
    shaerd_ptr<T>.

    TODO: Make thread-safe.

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
        shared_ptr<T>(shared_ptr<T> other_ptr);

        ~shared_ptr<T>();

        T &value();
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
        _shared = new shared_object();
        _shared->ref_count = 1;
        _shared->value = initial_value;
    }

    template <typename T>
    inline shared_ptr<T>::shared_ptr(shared_ptr<T> other_ptr)
    {
        _shared = other_ptr._shared;
        _shared->ref_count++;
    }

    template <typename T>
    inline shared_ptr<T>::~shared_ptr()
    {
        _shared->ref_count--;

        if (!_shared->ref_count)
            delete _shared;
    }

    template <typename T>
    inline T &shared_ptr<T>::value()
    {
        return *_value;
    }
}