LD R1, 0x01
.loop			#looping till r1 = 9
ADD	R1, 0x01
CMP R1, 0x09
BEQ end
BRA	loop
.end
HALT
