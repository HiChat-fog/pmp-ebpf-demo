# pmp-ebpf-demo

eBPF interpreter running unprivileged inside RISC-V PMP compartments on a CH32V307 (RV32IMAFC). Supervisor (M-mode) arms PMP, schedules compartments, and recovers after faults.

## Layout

- `ebpf/` - eBPF subset interpreter, policies, host tests
- `pmp/` - supervisor, monitor, attacker probe, trap handler, linker script
- `tools/inject_swarm.py` - swarm injection + host-side mirror check
- `reference/fw_pmp.bin` - firmware image used for recorded results
- `build.sh` - build everything

## Build

    ./build.sh

Output: `build/fw_pmp.bin`

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
