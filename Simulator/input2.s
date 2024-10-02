addi x2, x0, 8
addi x1, x2, -4
L2: jal x0, L1
add x0, x0, x0
L1: addi x2, x2, -1
bge x2, x0, L2
sub x0, x0, x0