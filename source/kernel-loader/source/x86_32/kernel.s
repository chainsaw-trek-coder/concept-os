    .section .rodata
    .global kernel_x86_32
    .global kernel_x86_32_size
kernel_x86_32:
    .incbin "kernel"
1:
kernel_x86_32_size:
    .int 1b - kernel_x86_32
