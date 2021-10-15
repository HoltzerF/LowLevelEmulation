#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include <windows.h>

// little endian memory
// instruction size 4
// ip contains instruction address relativ to memory for the next instruction
// program have to be terminated with halt instruction
// seven status registers to control program flow
// register width is two byte

#define INSSIZE 4	//instruction width
typedef uint8_t BYTE;    //byte
typedef uint16_t MWORD;    //machine word

//0 to 65,535 -> maximum addressable memory
BYTE memory[65536];

typedef struct{
    uint16_t offset;
    uint16_t size;
}program;

BYTE zerof;
BYTE carryf;

MWORD r[7], ip;

void print_status(){
	printf("registers:\n");
    int i;
    for(i = 0; i < 7; ++i){
        printf("R%d\t0x%04x\t%d\n", i, r[i], (uint16_t)r[i]);
    }
    printf("ZF = %d\nCF = %d\nIP = %d\n", zerof, carryf, ip);
}

void print_mem(MWORD address, MWORD size){
	printf("memory:");
    int i;
    for(i = 0; i < size; ++i){
    	if((i % 4) == 0){
        	putchar('\n');
		}
        printf("0x%02x ", memory[address + i]);
    }
    putchar('\n');
}

void check(void *mark, char *msg){
    if(mark == 0 || &mark == 0){
        fprintf(stderr, "error: %s", msg);
        exit(1);
    }
}

int fetch(){
    
    if(ip > 65535 - 4){
        //handle error
        return 1;
    }
    
    BYTE instructions[INSSIZE];
    int i;
    for(i = 0; i < INSSIZE; ++i){
        instructions[i] = memory[ip++];
    }
    
    //printf("exec: 0x%02x 0x%02x 0x%02x 0x%02x ", instructions[0], instructions[1], instructions[2], instructions[3]);
    
    MWORD add;
    switch(instructions[0]){
        case 0x00:	//load value into register
            *(BYTE *)&r[instructions[1]] = instructions[2];
            *((BYTE *)&r[instructions[1]] + 1) = instructions[3];
            zerof = 0;
    		carryf = 0;
            break;
        case 0x01:	//load value from other register into register
        	r[instructions[1]] = r[instructions[2]];
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x02: //load value from address into register
        	*((BYTE *)&add) = instructions[2];
        	*((BYTE *)&add + 1) = instructions[3];
        	*(BYTE *)&r[instructions[1]] = *((BYTE *)&memory[add]);
            *((BYTE *)&r[instructions[1]] + 1) = *((BYTE *)&memory[add] + 1);
            zerof = 0;
    		carryf = 0;
            break;
        case 0x10: //store value of register into address
        	*((BYTE *)&add) = instructions[2];
        	*((BYTE *)&add + 1) = instructions[3];
        	*((BYTE *)&memory[add]) = *((BYTE *)&r[instructions[1]]);
        	*((BYTE *)&memory[add + 1]) = *((BYTE *)&r[instructions[1]] + 1);
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x11:	//store low byte of register into low
        	break;
        case 0x12:	//store high byte of register into memory
        	break;
        case 0x13: //store value of register into other register
        	r[instructions[2]] = r[instructions[1]];
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x20: //compares register values
        	if(r[instructions[1]] == r[instructions[2]]){
        		zerof = 1;
        		carryf = 0;
			}else if(r[instructions[1]] < r[instructions[2]]){
				zerof = 0;
				carryf = 1;
			}else if(r[instructions[1]] > r[instructions[2]]){
				zerof = 0;
				carryf = 0;
			}
        	break;
        case 0x21: //compares register value and value
        	*((BYTE *)&add) = instructions[2];
        	*((BYTE *)&add + 1) = instructions[3];
        	if(r[instructions[1]] == add){
        		zerof = 1;
        		carryf = 0;
			}else if(r[instructions[1]] > add){
				zerof = 0;
				carryf = 1;
			}else if(r[instructions[1]] < add){
				zerof = 0;
				carryf = 0;
			}
        	break;
        case 0x30: //jump if equal
        	if(zerof == 1 && carryf == 0){
        		*((BYTE *)&ip) = instructions[2];
        		*(((BYTE *)&ip) + 1) = instructions[3];
        		zerof = 0;
        		carryf = 0;
			}
        	break;
        case 0x31:	//jump if greater
        	if(zerof == 0 && carryf == 0){
        		*((BYTE *)&ip) = instructions[2];
        		*(((BYTE *)&ip) + 1) = instructions[3];
        		zerof = 0;
    			carryf = 0;
			}
        	break;
        case 0x32:	//jump if less
        	if(zerof == 0 && carryf == 1){
        		*((BYTE *)&ip) = instructions[2];
        		*(((BYTE *)&ip) + 1) = instructions[3];
        		zerof = 0;
    			carryf = 0;
			}
        	break;
        case 0x33:	//jump to address
        	*((BYTE *)&ip) = instructions[2];
        	*(((BYTE *)&ip) + 1) = instructions[3];
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x40:	//adding value to register value
        	*((BYTE *)&add) = instructions[2];
        	*((BYTE *)&add + 1) = instructions[3];
        	//printf("-0x%04x- ", add);
        	r[instructions[1]] += add;
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x41: //substract value from register value
        	*((BYTE *)&add) = instructions[2];
        	*((BYTE *)&add + 1) = instructions[3];
        	r[instructions[1]] -= add;
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x42: //add value from register to value from register
        	*((BYTE *)&add) = *((BYTE *)&r[instructions[2]]);
        	*((BYTE *)&add + 1) = *(((BYTE *)&r[instructions[2]]) + 1);
        	r[instructions[1]] += add;
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0x43: //subtract value from register from value of register
        	*((BYTE *)&add) = *((BYTE *)&r[instructions[2]]);
        	*((BYTE *)&add + 1) = *(((BYTE *)&r[instructions[2]]) + 1);
        	//printf("-0x%04x- ", add);
        	r[instructions[1]] -= add;
        	zerof = 0;
    		carryf = 0;
        	break;
        case 0xfe:
        	printf("program terminated\n");
        	return 1;
        case 0xff:
        	zerof = 0;
    		carryf = 0;
        	break;
        default: printf("error");
			return 1;
    }
    //putchar('\n');
    return 0;
}

void run(program *prog){
    ip = prog->offset;
    while(fetch() == 0){
    	/*
    	putchar('\n');
    	print_status();/**/
    	//_getch();
	}
}

program load(char *filename, uint16_t address){
	
    FILE *fptr = fopen(filename, "rb");
    check(fptr, "cannot open file");
    
    program prog;
    prog.offset = address;
    
    fseek(fptr, 0, SEEK_END);
    prog.size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    
    if(prog.size + prog.offset > 65535){
        check(0, "filesize too big");
    }
    
    MWORD start = address;
    if(fread(&memory[prog.offset], prog.size, 1, fptr) != 1){
    		check(0, "reading into memory");
	}
	
    if(address != 0){	//change jump offsets in relation to loading address
    	while(memory[start] != 0xfe){
    		if(memory[start] >= 0x30 && memory[start] <= 0x33){
    			printf("0x%02x ", memory[start]);
    			MWORD process;
    			*(BYTE *)&process = memory[start + 2];
    			*((BYTE *)&process + 1) = memory[start + 3];
    			process += address;
    			memory[start + 2] = *(BYTE *)&process;
    			memory[start + 3] = *((BYTE *)&process + 1);
			}
    		start += INSSIZE;
		}
	}
    
    fclose(fptr);
    return prog;
}

int main(){
	//presets
    ip = 0;
    zerof = 0;
    carryf = 0;
    
    int i;
    for(i = 0; i < 7; ++i){
    	r[i] = 0;
	}
	//end preset
	
	program prog = load("test.o", 0);
	run(&prog);
	
	putchar('\n');
    print_status();
    
    return 0;
}
