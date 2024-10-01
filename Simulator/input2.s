addi x2, x0, 8
addi x1, x2, -4
beq x0, x0, L1
add x0, x0, x0
L1: jalr x2, x1, 12