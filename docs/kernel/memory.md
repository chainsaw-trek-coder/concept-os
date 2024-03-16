# Memory

## Recommendations from Intel
- Use write combining for frame buffers. This allows writes to buffer and avoid overloading cache lines.
- Use uncacheable for structures that are read only once. This prevents them from unnecessarily evicting other structures.
  - We can probably use this for messages? 
