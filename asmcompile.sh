in="${1:-sample_rv64.s}"
out=$(basename "$in" .s)
riscv64-unknown-elf-gcc $in -o $out -nostdlib -march=rv64i
