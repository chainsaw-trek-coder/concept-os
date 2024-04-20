# Memory

## Recommendations from Intel
- Use write combining for frame buffers. This allows writes to buffer and avoid overloading cache lines.
- Use uncacheable for structures that are read only once. This prevents them from unnecessarily evicting other structures.
  - We can probably use this for messages? 

## Recommendations from StackOverflow.

- [Mark pages you DON'T want flushed from TLB as "Global"](https://stackoverflow.com/questions/41986862/x86-64-page-table-global-bit)https://stackoverflow.com/questions/41986862/x86-64-page-table-global-bit
  - This could be useful for Kernel memory and messages.

## Page Allocation

Memory available to the OS will be used as two big data structures. One for page tables and one for free pages.

Allocating a page:

1. Traverse free page structure for a suitable block (or set of blocks).
2. Remove free pages from free data structure.
3. Add pages to page table (or create page table).

Deallocating a page:

1. Remove page from page table (or remove page table).
2. Add page to free data structure.
3. Coalesce adjacent free pages if possible.

Functions to support these tasks include:

* Create page table.
* Allocate page to table.
* Deallocate page from table.
* Delete page table.

Question:

Do we need a top level record of all of the page tables?
So far the functions listed above do not appear to need one.

### Paging Data Structures

* Page tables as defined in Intel's manual is sufficient to represent allocated memory.
* Free page structures a collections of 4k blocks with number of 4k blocks as well as two sets of links. One set representing their relationship in a binary tree sorted by size while another set of links represent their relationship in a binary tree sorted by location.

If we need to have a top level record of all page tables, this should be a linked list of 4k nodes.
Maybe this could store process information.