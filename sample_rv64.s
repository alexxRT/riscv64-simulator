    .section .text
    .global _start             # Entry point

_start:
    # Init registers
    addi x20, x0, 10
    addi x11, x0, 20
#    li a0, 10                  # Load `10` to a0
#    li a1, 20                  # Load `20` to a1

    # Some arithmetics
    add x12, x20, x11             # a2 = a0 + a1 (10 + 20 = 30)
    sub x13, x11, x20             # a3 = a1 - a0 (20 - 10 = 10)
    # not implemented yet
    #mul a4, a0, a1             # a4 = a0 * a1 (10 * 20 = 200)
    #div a5, a1, a0             # a5 = a1 / a0 (20 / 10 = 2)


    # Prepare to exit
    li a7, 93                  # Syscall "exit" code
    li a0, 0                   # Return code `0`
    ecall                      # Do syscall
