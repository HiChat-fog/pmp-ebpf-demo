
#include <stdint.h>

#define MON_JIEGUO ((volatile uint32_t *)0x20004200u)
#define MON_SHURU   ((volatile uint32_t *)0x20004080u)
#define ATK_ZIQU     ((volatile uint32_t *)0x20008000u)   
#define ATK_ZHENGMING   ((volatile uint32_t *)0x20008008u)   
#define MON_FLASH   0x08002000u                          

void gongji_zhu(void) {
    *ATK_ZIQU = 0xBEEF5EEDu;      
    *MON_JIEGUO = 0x77777777u;  
    *MON_SHURU = 0x88888888u;    
    {
        volatile uint32_t v = *MON_JIEGUO;  
        (void)v;
    }

    __asm__ volatile (
        ".option push\n.option norvc\n"
        "jalr x0, 0(%0)\n"
        ".option pop\n" :: "r" (MON_FLASH) : "memory");
    for (;;) { __asm__ volatile ("wfi"); }   
}

void gongji_huifu(void) {
    *ATK_ZHENGMING = 0x5EEDC0DEu;    
    __asm__ volatile ("ecall");  
    for (;;) { __asm__ volatile ("wfi"); }
}

asm(
    ".section .attacker.text\n"
    ".globl gongji_rukou\n"
    ".align 2\n"
    "gongji_rukou:\n"
    "  lui  sp, %hi(_atk_stack_top)\n"
    "  addi sp, sp, %lo(_atk_stack_top)\n"
    "  lui  t0, %hi(gongji_zhu)\n"
    "  addi t0, t0, %lo(gongji_zhu)\n"
    "  jalr t0\n"
    "1: wfi\n"
    "  j 1b\n"
);
