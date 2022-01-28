
Repository just for practice

- everything still work in progress
- instrution set designed by Gary Explains https://www.youtube.com/channel/UCRjSO-juFtngAeJGJRMdIZw

cpu:
- little endian memory
- instruction size 4 bytes
- instruction pointer (ip) contains instruction address relativ to memory for the next instruction
- program have to be terminated with halt instruction
- seven status registers to control program flow
- register width is two byte
- opcodes always work on registers

assembler:
- lasm: a basic two pass assembler for the instruction set
- simply converts the .asm to a binary programm which can be load and executed by the cpu

assembly
- look at InstructionSet.png to see the available instructions
- instuctions always operate on registers
- assembler supports simple labels/branches like .NAME which can be used by the branch operations
- and a simple comments starting with #
- number can be decimal 3213 or hexadecimal 0x...

test:
- test.asm is a simple assembler programm written for the cpu and can be translated with lasm
