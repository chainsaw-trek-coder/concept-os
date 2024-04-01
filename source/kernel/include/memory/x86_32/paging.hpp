#ifndef header_57f15ad7_5e7f_4313_a12e_65971fa13977
#define header_57f15ad7_5e7f_4313_a12e_65971fa13977

enum page_entry_type
{
    user = 1,
    supervisor = 2
};

struct page_table_entry
{
    unsigned data;

    page_table_entry();

    void set_address(void* address);
    void* get_address();
    bool is_global_page();
    void set_global_page(bool is_global_page);
    short page_table_attribute_index();
    bool is_dirty();
    void set_dirty(bool is_dirty);
    bool is_accessed();
    void clear_accessed();
    bool is_cache_disabled();
    bool is_write_through();
    void set_type(page_entry_type type);
    page_entry_type get_type();
    bool is_writable();
    void set_writable(bool is_writable);
    bool is_present();
    void set_present(bool is_present);
};

struct page_table
{
    page_table_entry entries[1024];
};

struct page_directory_entry
{ 
    unsigned data;

    page_directory_entry();

    void set_address(page_table *address);
    page_table *get_address();
    bool is_4mb_page();
    bool is_accessed();
    void clear_accessed();
    bool is_cache_disabled();
    bool is_write_through();
    page_entry_type get_type();
    bool is_writable();
    void set_writable(bool is_writable);
    bool is_present();
    void set_present(bool is_present);
};

struct page_directory
{
    page_directory_entry entries[1024];
};

#endif
