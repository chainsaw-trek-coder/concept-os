#ifndef f1076100_e61a_4fc1_984f_93f0241674cf
#define f1076100_e61a_4fc1_984f_93f0241674cf

template<typename T>
class auto_delete
{
private:
    T* data;

public:
    auto_delete(T *data);
    ~auto_delete();
};

template <typename T>
inline auto_delete<T>::auto_delete(T *data)
{
    this->data = data;
}

template <typename T>
inline auto_delete<T>::~auto_delete()
{
    delete data;
}

#endif