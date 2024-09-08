add x5, x1, x2
sub x6, x3, x4  
label10: and x7, x5, x6  
or x8, x7, x0  
xor x9, x1, x3  
sll x10, x2, x1  
label11: srl x11, x6, x2  
sra x12, x5, x4  
add x13, x11, x9  
sub x14, x8, x7  
and x15, x13, x12  
or x16, x10, x14  
xor x17, x3, x8  
sll x18, x7, x1  
label9: srl x19, x9, x5  
sra x20, x16, x6  
add x21, x15, x17  
sub x22, x19, x20  
and x23, x21, x11  
label12: or x24, x18, x22  
addi x5, x1, 5  
slli x9, x5, 3  
srli x10, x6, 2  
srai x11, x7, 1  
addi x12, x8, -10  
label4: slli x16, x12, 2  
srli x17, x13, 4  
label17: srai x18, x14, 5  
addi x19, x15, 20  
slli x23, x19, 1  
srli x24, x20, 3  
ld x5, 0(x1)  
label13: lw x6, 4(x2)  
lh x7, 8(x3)  
lb x8, 12(x4)  
lwu x9, 16(x5)  
label18: lhu x10, 20(x6)  
lbu x11, 24(x7)  
ld x12, 32(x8)  
lw x13, 36(x9)  
lh x14, 40(x10)  
lb x15, 44(x11)  
lwu x16, 48(x12)  
lhu x17, 52(x13)  
label3: lbu x18, 56(x14)  
ld x19, 60(x15)  
label5: lw x20, 64(x16)  
lh x21, 68(x17)  
lb x22, 72(x18)  
lwu x23, 76(x19)  
lhu x24, 80(x20)  
sd x1, 0(x2)  
sw x3, 4(x4)  
label2: sh x5, 8(x6)  
sb x7, 12(x8)  
label14: sd x9, 16(x10)  
label1: sw x11, 20(x12)  
sh x13, 24(x14)  
sb x15, 28(x16)  
sd x17, 32(x18)  
sw x19, 36(x20)  
label6: sh x21, 40(x22)  
sb x23, 44(x24)  
sd x25, 48(x0)  
sw x27, 52(x1)  
sh x29, 56(x2)  
sb x31, 60(x3)  
sd x4, 64(x5)  
sw x6, 68(x7)  
sh x8, 72(x9)  
label15: sb x10, 76(x11)  
beq x1, x2, label1  
bne x3, x4, label2  
blt x5, x6, label3  
bge x7, x8, label4  
bltu x9, x10, label5  
bgeu x11, x12, label6  
label7: beq x13, x14, label7  
bne x15, x16, label8  
blt x17, x18, label9  
bge x19, x20, label10  
bltu x21, x22, label11  
label8: bgeu x23, x24, label12  
beq x25, x26, label13  
bne x27, x28, label14  
blt x29, x30, label15  
bge x31, x0, label16  
jal x1, label17  
jal x2, label18  
lui x3, 0x10000  
label16: lui x4, 0x20000  