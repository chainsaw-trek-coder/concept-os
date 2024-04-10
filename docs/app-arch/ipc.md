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

# IPC Message

Messages start with a header the number of parameters, offsets into the block of memory for each parameter
paired with the size of the parameter. This is followed by the values for the parameters.

 -------------------------------------------------------------------------------------------------------------
| Number of parameter | Parameter offset 1 | Parameter size 1  | Parameter offset 2 | Parameter size 2 | ...  |
 -------------------------------------------------------------------------------------------------------------
| Parameter value 1   | Parameter value 2 | ...                                                               | 
 -------------------------------------------------------------------------------------------------------------

Offsets can have 3 states:
1. If offset is greater than 0, the parameter is set.
2. If the offset is 0, the parameter is determined to be null.

The intent of this scheme is to make it easy to fill structure quickly and to read it quickly.

When constructing the message, the client:

1. Initializes the header with the number of parameters, followed by zero's for offsets/sizes.
2. As parameters are set the values are stored after the header and the correct offset/size of that value is set.
