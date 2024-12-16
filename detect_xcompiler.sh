#!/bin/sh

if ! command -v riscv64-elf-gcc 2>&1 >/dev/null
then
    echo riscv64-unknown-elf-gcc
else
    echo riscv64-elf-gcc
fi
