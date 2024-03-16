# Concept OS

Recommended Reading:

[Intel Documentation](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

[Intel x86/64 Documentation Volume 3A](https://www.intel.com/content/dam/support/us/en/documents/processors/pentium4/sb/25366821.pdf#page72)

[Intel x86/64 Documentation Combined Volume](https://cdrdv2-public.intel.com/812392/325462-sdm-vol-1-2abcd-3abcd-4.pdf)

[ELF Documentation](https://wiki.osdev.org/ELF)

Architecture

- Everything is a service.
- No concept of an "admin" or "root".
- Timers normally used for multi-tasking are only used to time out threads.
- "Zero-tick" scheduling
- Anonymous users are those that are not logged in.
- OS starts logged in as anonymous.
- Most apps are treated equally.

For Future Reference:

[Embedding files in ELF.](https://stackoverflow.com/questions/5479691/is-there-any-standard-way-of-embedding-resources-into-linux-executable-image)

[Doug Lea's Malloc](https://gee.cs.oswego.edu/pub/misc)

[Debugging Kernel in QEMU](https://wiki.osdev.org/How_Do_I_Use_A_Debugger_With_My_OS)
