enum page_directory_entry_type
{
    user,
    supervisor
};

struct page_table_entry
{
    unsigned data;

    // bool is_global_page();
    // bool page_table_attribute_index();
    // bool is_dirty();
    // bool is_accessed();
    // bool is_cache_disabled();
    // bool is_write_Through();
    // page_directory_entry_type get_type();
    // bool is_writable();
    // bool is_present();
};

struct page_directory_entry
{
    unsigned data;

    void set_address(page_table_entry* address);
    page_table_entry* get_address();
    bool is_4mb_page();
    bool is_accessed();
    void clear_accessed();
    // bool is_cache_disabled();
    // bool is_write_through();
    // page_directory_entry_type get_type();
    // bool is_writable();
    // bool is_present();
};