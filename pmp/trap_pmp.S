
    .section .text
    .align 4
    .globl xianjing_rukou
xianjing_rukou:
    csrr t0, mcause
    li   t1, 8
    beq  t0, t1, .Lecall

    lui  t3, %hi(g_ri_xuhao)
    addi t3, t3, %lo(g_ri_xuhao)
    lw   t4, 0(t3)                      
    slli t5, t4, 2                      

    csrr t6, mtval                      

    lui  a0, %hi(g_ri_yuanyin)
    addi a0, a0, %lo(g_ri_yuanyin)
    add  a0, a0, t5
    sw   t0, 0(a0)                      

    lui  a1, %hi(g_ri_zhi)
    addi a1, a1, %lo(g_ri_zhi)
    add  a1, a1, t5
    sw   t6, 0(a1)                      

    addi t4, t4, 1
    li   t1, 8
    blt  t4, t1, .Lidx_ok
    li   t4, 7                          
.Lidx_ok:
    sw   t4, 0(t3)

    li   t1, 1
    beq  t0, t1, .Lredirect             

    csrr t1, mepc
    lhu  t2, 0(t1)
    andi t2, t2, 3
    li   t3, 2
    beq  t2, t3, .Lskip2
    li   t3, 4
.Lskip2:
    add  t1, t1, t3
    csrw mepc, t1
    mret                                

.Lredirect:
    lui  t4, %hi(g_zhiding)
    addi t4, t4, %lo(g_zhiding)
    lw   t4, 0(t4)
    csrw mepc, t4
    mret                                

.Lecall:
    lui  sp, %hi(_sup_stack_top)
    addi sp, sp, %lo(_sup_stack_top)
    lui  t0, %hi(guanli_huifu)
    addi t0, t0, %lo(guanli_huifu)
    csrw mepc, t0
    csrr t1, mstatus
    li   t2, 0x1800                     
    or   t1, t1, t2
    csrw mstatus, t1
    mret
