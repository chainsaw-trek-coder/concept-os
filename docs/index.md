# Concept OS

Recommended Reading:

[Intel x86/64 Documentation](https://www.intel.com/content/dam/support/us/en/documents/processors/pentium4/sb/25366821.pdf)

[ELF Documentation](https://wiki.osdev.org/ELF)

Architecture

- Everything is a service.
- No concept of an "admin" or "root".
- Timers normally used for multi-tasking are only used to time out threads.
- Anonymous users are those that are not logged in.
- OS starts logged in as anonymous.
- Most apps are treated equally.

For Future Reference:

[Embedding files in ELF.](https://stackoverflow.com/questions/5479691/is-there-any-standard-way-of-embedding-resources-into-linux-executable-image)