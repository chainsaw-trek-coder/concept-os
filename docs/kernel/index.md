# Kernel

## Boot sequence

1. Setup CPU.
2. Setup memory.
3. Load Text mode driver.
    - Create page directory for driver.
        - Allocate page directory.
        - Allocate page tables.
        - Allocate pages.
    - Load text mode driver binary into page directory.
    - Create application structures.
        - Add link to page directory.
    - Set CR3 register to page directory.
4. Text mode driver triggers loading of Terminal UI.
5. Load Default Storage Driver.
6. Loading of Storage Driver triggers loading of File System Driver.
7. Load Package manager
8. Load Hardware Manager.
    - Hardware manager scans devices.
    - Hardware manager loads relevant drivers from package manager.
    - Some drivers replace others. (e.g. Video driver replaces Text mode driver.)


## Page management

Perhaps page allocator will treat all the free pages as one giant AVL tree.

Used pages will be grouped into page directories.

This all depends on the needs of the kernel for executing kernel level code.