# Memory

## Recommendations from Intel
- Use write combining for frame buffers. This allows writes to buffer and avoid overloading cache lines.
- Use uncacheable for structures that are read only once. This prevents them from unnecessarily evicting other structures.
  - We can probably use this for messages? 

## Recommendations from StackOverflow.

- [Mark pages you DON'T want flushed from TLB as "Global"](https://stackoverflow.com/questions/41986862/x86-64-page-table-global-bit)https://stackoverflow.com/questions/41986862/x86-64-page-table-global-bit
  - This could be useful for Kernel memory and messages.
