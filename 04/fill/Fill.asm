// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// R0 = key
// R1 = counter

(START)
@KBD
D=M
@R0
M=D      // write key to R0
@SCREEN
D=A
@8191
D=D+A
@R1
M=D      // counter = SCREEN + 8191
(LOOP)
@R1
D=M
@SCREEN
D=D-A
@START
D;JLT    // if (counter < SCREEN) goto START
@R0
D=M
@FILL0
D;JEQ    // if (key == 0) goto FILL0
@R1
A=M
M=-1     // *counter = 0xFFFF
@DECR
0;JMP
(FILL0)
@R1
A=M
M=0      // *counter = 0
(DECR)
@R1
M=M-1    // decrement counter
@LOOP
0;JMP
