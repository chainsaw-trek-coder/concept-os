template <typename T>
class linked_list_node
{

private:
    linked_list_node<T>* prev;
    linked_list_node<T>* next;

};

template <typename T>
class linked_list
{

private:
    linked_list_node<T>* head;

};