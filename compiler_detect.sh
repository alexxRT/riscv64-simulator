#!/bin/sh

IFS=":" read -ra paths <<< "$PATH"
find "${paths[@]}" -name 'riscv64*elf*gcc' -perm -o=x,-g=x,-u=x
