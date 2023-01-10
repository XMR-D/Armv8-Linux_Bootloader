.section ".text.startup"

.global _start

_start:
    ldr x30, =STACK_TOP
    mov sp, x30
    mov x6, x0
    bl init_neon
    mov x0, x6
    bl Decypher
    ret
.hang:
    b .hang

init_neon:
    mrs x1, cpacr_el1
    mov x0, #(3 << 20)
    orr x0,x1,x0
    msr cpacr_el1, x0
    ret
