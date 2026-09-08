    .section .init, "ax"
    .globl _start
_start:

    lui     sp, %hi(_stack_top)
    addi    sp, sp, %lo(_stack_top)

    lui     t0, %hi(__bss_start)
    addi    t0, t0, %lo(__bss_start)
    lui     t1, %hi(__bss_end)
    addi    t1, t1, %lo(__bss_end)
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    call    rukou
3:
    wfi
    j       3b
