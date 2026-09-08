#include <stdint.h>

#define STATUS  ((volatile uint32_t *)0x20000100u)
#define GEQU    ((volatile uint32_t *)0x20001000u)
#define WAI     ((volatile uint32_t *)0x20006000u)

#define PEIZHI_RW   0x1Bu
#define PEIZHI_JIN  0x18u
#define GEQU_MA   ((0x20001000u >> 2) | (8192u >> 3) - 1u)
#define JIN_MA    ((0x20006000u >> 2) | (8192u >> 3) - 1u)

static inline uint32_t csr_du(uint32_t addr) {
    uint32_t v;
    __asm__ volatile ("csrr %0, %1" : "=r"(v) : "i"(addr));
    return v;
}
static inline void csr_xie(uint32_t addr, uint32_t v) {
    __asm__ volatile ("csrw %1, %0" :: "r"(v), "i"(addr) : "memory");
}

volatile uint32_t rizhi[17];
uint32_t g_jieduan;

void guanli_fapai(void);
extern void u_cungeng(void);
extern void xiangying(void);

static void bufang_jinbi(void) {
    csr_xie(0x3a0, 0u);
    csr_xie(0x3b0, GEQU_MA);
    csr_xie(0x3b1, JIN_MA);
    csr_xie(0x3b2, 0u);
    csr_xie(0x3b3, 0u);
    csr_xie(0x3a0, (uint32_t)PEIZHI_RW | ((uint32_t)PEIZHI_JIN << 8));
}

static void jin_yonghu(void) {
    uint32_t ms = csr_du(0x300);
    ms &= ~(3u << 11);
    csr_xie(0x300, ms);
    csr_xie(0x341, (uint32_t)u_cungeng);
    __asm__ volatile ("mret");
    for (;;) { __asm__ volatile ("wfi"); }
}

void guanli_fapai(void) {
    STATUS[40] = rizhi[0];
    for (int i = 0; i < 16; ++i) STATUS[41 + i] = rizhi[1 + i];
    STATUS[60] = g_jieduan;
    if (g_jieduan == 1) {
        bufang_jinbi();
        GEQU[2] = 0x102u;
        g_jieduan = 2;
        jin_yonghu();
    } else if (g_jieduan == 2) {
        STATUS[59] = 0xC0DE0002u;
        GEQU[3] = 0x51515151u;
        csr_xie(0x305, ((uint32_t)xiangying) | 2u);
        GEQU[2] = 0x103u;
        g_jieduan = 3;
        jin_yonghu();
    } else {
        csr_xie(0x305, (uint32_t)xiangying);
        STATUS[2] |= 0x8000u;
        for (;;) {
            STATUS[3]++;
            for (volatile int k = 0; k < 20000; ++k) __asm__ volatile ("nop");
        }
    }
}

void tan_zhen_zhu(void) {
    for (int i = 2; i <= 96; ++i) STATUS[i] = 0u;
    rizhi[0] = 0u;
    for (int i = 1; i <= 16; ++i) rizhi[i] = 0u;
    STATUS[0] = 0xDEADBEEFu;
    STATUS[1] = 0xC0DE00A1u;
    csr_xie(0x305, (uint32_t)xiangying);

    csr_xie(0x3a0, 0x1F1F1F1Fu);
    STATUS[10] = 0x1F1F1F1Fu;
    STATUS[11] = csr_du(0x3a0);
    csr_xie(0x3a1, 0x1F1F1F1Fu);
    STATUS[12] = 0x1F1F1F1Fu;
    STATUS[13] = csr_du(0x3a1);

    csr_xie(0x3b0, 0xAAAAAAAAu); STATUS[14] = 0xAAAAAAAAu; STATUS[15] = csr_du(0x3b0);
    csr_xie(0x3b1, 0x11111111u); STATUS[24] = 0x11111111u; STATUS[25] = csr_du(0x3b1);
    csr_xie(0x3b2, 0x22222222u); STATUS[26] = 0x22222222u; STATUS[27] = csr_du(0x3b2);
    csr_xie(0x3b3, 0x33333333u); STATUS[28] = 0x33333333u; STATUS[29] = csr_du(0x3b3);
    csr_xie(0x3b4, 0x55555555u); STATUS[16] = 0x55555555u; STATUS[17] = csr_du(0x3b4);
    csr_xie(0x3b5, 0xDEADBEEFu); STATUS[18] = 0xDEADBEEFu; STATUS[19] = csr_du(0x3b5);
    csr_xie(0x3b6, 0x12345678u); STATUS[20] = 0x12345678u; STATUS[21] = csr_du(0x3b6);
    csr_xie(0x3b7, 0x87654321u); STATUS[22] = 0x87654321u; STATUS[23] = csr_du(0x3b7);

    STATUS[30] = csr_du(0x3a2);
    STATUS[31] = csr_du(0x3a3);
    csr_xie(0x3a2, 0x1F1F1F1Fu);
    STATUS[32] = csr_du(0x3a2);
    csr_xie(0x3a2, 0u);

    {
        uint32_t orig = csr_du(0x305);
        STATUS[33] = orig;
        csr_xie(0x305, orig | 2u);
        STATUS[34] = csr_du(0x305);
        csr_xie(0x305, orig | 1u);
        STATUS[35] = csr_du(0x305);
        csr_xie(0x305, orig);
        STATUS[36] = csr_du(0x305);
    }

    csr_xie(0x3a0, 0u);
    csr_xie(0x3b0, GEQU_MA);
    csr_xie(0x3b1, 0u);
    csr_xie(0x3b2, 0u);
    csr_xie(0x3b3, 0u);
    csr_xie(0x3a0, PEIZHI_RW);
    *(volatile uint32_t *)0x20007000u = 0x5A5A5A5Au;
    __asm__ volatile ("fence iorw, iorw" ::: "memory");
    STATUS[58] = *(volatile uint32_t *)0x20007000u;

    GEQU[0] = 0u; GEQU[1] = 0u; GEQU[2] = 0x101u;
    WAI[0] = 0u;
    WAI[1] = 0x0BAD0BADu;
    g_jieduan = 1;
    jin_yonghu();
    for (;;) { __asm__ volatile ("wfi"); }
}

__asm__(
    ".section .text.startup,\"ax\",@progbits\n"
    ".globl _start\n"
    "_start:\n"
    "  lui  sp, %hi(_stack_top)\n"
    "  addi sp, sp, %lo(_stack_top)\n"
    "  j    tan_zhen_zhu\n"
    "1: wfi\n"
    "  j 1b\n"
);

__asm__(
    ".text\n"
    ".align 2\n"
    ".globl u_cungeng\n"
    "u_cungeng:\n"
    "  li   t0, 0x20001000\n"
    "  li   t1, 0xA1A1A1A1\n"
    "  sw   t1, 0(t0)\n"
    "  li   t0, 0x20006000\n"
    "  li   t1, 0xCAFE0002\n"
    "  sw   t1, 0(t0)\n"
    "  li   t0, 0x20001000\n"
    "  li   t1, 0xA2A2A2A2\n"
    "  sw   t1, 0(t0)\n"
    "  li   t0, 0x20006004\n"
    "  lw   t2, 0(t0)\n"
    "  li   t0, 0x20001000\n"
    "  li   t1, 0xA3A3A3A3\n"
    "  sw   t1, 0(t0)\n"
    "  sw   t2, 4(t0)\n"
    "  li   t1, 0xA4A4A4A4\n"
    "  sw   t1, 0(t0)\n"
    "  ecall\n"
    "1: wfi\n"
    "  j 1b\n"
);

__asm__(
    ".text\n"
    ".align 2\n"
    ".globl xiangying\n"
    "xiangying:\n"
    "  csrr t0, mcause\n"
    "  li   t1, 8\n"
    "  beq  t0, t1, .Lgom\n"
    "  lui  t2, %hi(rizhi)\n"
    "  addi t2, t2, %lo(rizhi)\n"
    "  lw   t3, 0(t2)\n"
    "  li   t5, 8\n"
    "  bge  t3, t5, .Lskip\n"
    "  slli t4, t3, 3\n"
    "  addi t4, t4, 4\n"
    "  add  t4, t4, t2\n"
    "  csrr t5, mtval\n"
    "  sw   t0, 0(t4)\n"
    "  sw   t5, 4(t4)\n"
    "  addi t3, t3, 1\n"
    "  sw   t3, 0(t2)\n"
    ".Lskip:\n"
    "  csrr t1, mepc\n"
    "  lhu  t2, 0(t1)\n"
    "  andi t2, t2, 3\n"
    "  addi t1, t1, 2\n"
    "  li   t3, 3\n"
    "  bne  t2, t3, .Lskipd\n"
    "  addi t1, t1, 2\n"
    ".Lskipd:\n"
    "  csrw mepc, t1\n"
    "  mret\n"
    ".Lgom:\n"
    "  lui  t0, %hi(guanli_fapai)\n"
    "  addi t0, t0, %lo(guanli_fapai)\n"
    "  csrw mepc, t0\n"
    "  csrr t1, mstatus\n"
    "  li   t2, 0x1800\n"
    "  or   t1, t1, t2\n"
    "  csrw mstatus, t1\n"
    "  mret\n"
);
