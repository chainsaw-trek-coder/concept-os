# Interprocess Communication

Most messages between applications will be passed via shared memory segments.

The first call workflow involves:
1. Caller requests "connection" to callee.
2. OS validates caller's permission to connect to callee.
3. Callee validates caller's permission to connect to callee.
4. Connection established.
5. Metadata about supported functions may be passed from callee to caller.
6. Caller requests space for message from OS.
7. OS creates a shared memory block and passes it back to caller.
8. Caller constructs message on shared memory block.
9. Caller instructs OS to pass shared memory block to callee.
10. OS makes shared memory block read only to caller.
11. OS maps shared memory block into callee's address space.
12. OS calls callee passing in the shared memory block.
13. Callee executes caller's request.
14. Callee requests shared memory block from OS for return if not an integer.
15. OS creates and maps a shared memory block into callee's address space.
16. Callee constructs return message on shared memory block.
17. Callee instructs OS that it is done with caller's message.
18. OS returns caller's shared memory block back to pool of shared memory blocks.
19. Callee instructs OS to return shared memory block to caller.
20. OS marks shared memory block as read only to callee.
21. OS maps return shared memory block into caller's address space.
22. OS calls caller to finish execution passing in the return shared memroy block.
23. Caller processes response.
24. Caller instructs the OS that it is done with the return message.
25. Caller instructs the OS that it is done with the caller's shared memory block.
26. OS releases return shared memory block to pool of shared memory blocks.