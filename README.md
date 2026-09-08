# pmp-ebpf-demo

eBPF interpreter running unprivileged inside RISC-V PMP compartments on a CH32V307 (RV32IMAFC). Supervisor (M-mode) arms PMP, schedules compartments, and recovers after faults.

## Layout

- `ebpf/` - eBPF subset interpreter, policies, host tests
- `pmp/` - supervisor, monitor, attacker probe, trap handler, linker script, PMP semantics probe
- `tools/inject_swarm.py` - swarm injection + host-side mirror check
- `reference/fw_pmp.bin` - firmware image used for recorded results
- `build.sh` - build everything

## Build

    ./build.sh

Output: `build/fw_pmp.bin`, `build/pmp_probe.bin`

## PMP semantics probe

    wlink flash -e build/pmp_probe.bin

Results land in SRAM: CSR read-backs and fault log at 0x20000100, U-mode markers at 0x20001000 (`wlink dump 0x20000100 400`). The last stage ends hung by design (misaligned-mtvec trap test); reflash `build/fw_pmp.bin` afterwards.

## Host tests

    cd ebpf
    gcc -fsanitize=address,undefined -I. test_host.c ebpf_mini.c helpers_rv32.c -o t
    ./t

## Run on board

Flash with wlink, then:

    wlink flash -e build/fw_pmp.bin
    python3 tools/inject_swarm.py 32 --seed 7

## Toolchain

clang 18 (riscv32), rust-lld, riscv64-unknown-elf-objcopy, wlink
