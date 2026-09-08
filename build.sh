#!/usr/bin/env bash
set -e
CC="clang --target=riscv32 -march=rv32imafc -mabi=ilp32 -mno-relax -msmall-data-limit=0 -ffreestanding -O2"
LLD="${LLD:-$HOME/.rustup/toolchains/nightly-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/bin/rust-lld}"
# 对象文件须以 monitor.o / attacker.o 基名参与链接（链接脚本按文件名分配舱室段）
mkdir -p build
$CC -c ../startup.S -o startup.o
$CC -c ../pmp/trap_pmp.S -o trap_pmp.o
$CC -c ../pmp/main_pmp.c -o main_pmp.o
$CC -c ../pmp/monitor_main.c -o monitor.o
$CC -O0 -c ../pmp/attacker_main.c -o attacker.o   # attacker -O0: live values in frame (see header)
$LLD -flavor gnu -T ../pmp/link_pmp.ld -nostdlib -o fw_pmp.elf \
    startup.o main_pmp.o monitor.o attacker.o trap_pmp.o
riscv64-unknown-elf-objcopy -O binary fw_pmp.elf fw_pmp.bin
echo "build/fw_pmp.bin"
