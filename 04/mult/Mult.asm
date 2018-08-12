// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// R0 = multiplicand
// R1 = multiplier
// R2 = product
// R3 = copy of multiplier

@R2
M=0      // initialize product to 0
@R1
D=M
@R3
M=D      // copy multiplier to R3
(LOOP)
@R3
D=M
@END
D;JLE    // if (multiplier <= 0) goto END
@R2
D=M
@R0
D=D+M
@R2
M=D      // product += multiplicand
@R3
M=M-1    // decrement multiplier
@LOOP
0;JMP
(END)
@END
0;JMP    // infinite loop