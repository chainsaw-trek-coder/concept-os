# Kernel

## Boot sequence

1. Setup CPU.
2. Setup memory.
3. Start scheduler
4. Load Text mode driver.
    - Create page directory for driver.
        - Allocate page directory.
        - Allocate page tables.
        - Allocate pages.
    - Load text mode driver binary into page directory.
    - Create application structures.
        - Add link to page directory.
5. Text mode driver triggers loading of Terminal UI.
6. Load Default Storage Driver.
7. Loading of Storage Driver triggers loading of File System Driver.
8. Load Package manager
9. Load Hardware Manager.
    - Hardware manager scans devices.
    - Hardware manager loads relevant drivers from package manager.
    - Some drivers replace others. (e.g. Video driver replaces Text mode driver.)


## Page management

Perhaps page allocator will treat all the free pages as one giant AVL tree.

Used pages will be grouped into page directories.

This all depends on the needs of the kernel for executing kernel level code.

## Application memory layout

1. GDT
2. IDT
3. System call code
4. System stack
5. Application memory.

## Handling interrupt or system call.

1. Processor jumps system function mapped into application address space.
2. System function immediately disables paging.
3. System handles call.
4. Paging re-enabled.
5. Return to application.

## Task switching

1. Timer interrupt occurs.
2. System immediately disables paging.
3. System saves return information into current task structures.
4. System loads return information from new task structure.
5. System sets CR3 register for new task structure.
6. System re-enables paging.
7. System returns to application.
