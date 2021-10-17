#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX 256
#define INSWIDTH 4
#define OPCODES 13

struct _mark{
	int address;
	char *name;
	struct _mark *next;
};
typedef struct _mark mark;

int iswhitespace(char *c){
	return (*c == ' ' || *c == '\n' || *c == '\t');
}

typedef uint8_t BYTE;
typedef uint16_t MWORD;

mark *table;

char *commands[] = {
	"LD",
	"ST",
	"STL",
	"STH",
	"CMP",
	"BEQ",
	"BGT",
	"BLT",
	"BRA",
	"ADD",
	"SUB",
	"HALT",
	"NOOP"
};

int power(int base, int n){
    int p = 1;
    while (n--)
        p = p * base;
    return p;
}

int getnum(char *ptr){
	char *end = ptr;
	MWORD num = 0;
	
	//hexadecimal
	if ((*end == '0') && (*(end + 1) == 'x')){
		ptr += 1;
		
		while(iswhitespace(end) == 0){
			end++;
		}
		--end;
	
		int index = 0;
		while(end != ptr){
			if(*end >= 65 && *end <= 70){
				num += power(16, index) * (*end -55);
			}else{
				num += power(16, index) * (*end - 48);
			}
			index++;
			--end;
		}	
	}else{	//integer value
		while(iswhitespace(end) == 0){
			*end++;
		}
		
		int pot = 0;
		do{
			num += power(10, pot) * (*--end - 48);
			++pot;
		}while(ptr != end);
	}
	return num;
}

MWORD getAddressByLabel(char *ptr){
	char buffer[50];
	int i = 0;
	while(iswhitespace(ptr) == 0){
		buffer[i] = *ptr;
		++i;
		++ptr;
	}
	buffer[i] = '\0';
	
	mark *m = table;
	while(m != NULL){
		if(strcmp(buffer, m->name) == 0){
			return m->address;
		}
		m = m->next;
	}
}

void check(void *mark, char *msg){
    if(mark == 0 || &mark == 0){
        fprintf(stderr, "error: %s", msg);
        exit(1);
    }
}

void print_list(){
	mark *ptr = table;
	while(ptr->next != NULL){
		printf("%s %d\n", ptr->name, ptr->address);
		ptr = ptr->next;
	}
	printf("%s %d\n", ptr->name, ptr->address);
}

void addtable(char *name, int address){
	if(table == NULL){
		table = malloc(sizeof(mark));
		check(table, "cannot allocate memory");
		
		table->address = address;
		table->name = name;
		table->next = NULL;
	}else{
		mark *ptr = table;
		while(ptr->next != NULL){
			ptr = ptr->next;
		}
		ptr->next = malloc(sizeof(mark));
		check(ptr->next, "cannot allocate memory");
		
		ptr = ptr->next;
		ptr->address = address;
		ptr->name = name;
		ptr->next = NULL;
	}
}

void destroytable(){
	check(table, "no list");
}

int main(int argc, char *argv[]){
	
	if(argc < 2){
		fprintf(stderr, "error: usage: lasm.exe 'InputFile'");
		return 1;
	}
	
	table = NULL;
	
	FILE *fptr = fopen(argv[1], "r");
	check(fptr, "cannot open file");
	
	char *fname = malloc(sizeof(argv[1]) + 1);
	check(fname, "cannot allocate memory");
	strcpy(fname, argv[1]);
	fname[sizeof(argv[1]) - 2] = '\0';
	fname[sizeof(argv[1]) - 3] = 'o';
	FILE *out = fopen(fname, "w");
	check(out, "cannot open file");
	
	printf("input: %s - output: %s", argv[1], fname);
	free(fname);
	
	char buffer[MAX];
	int linecount = 0;
	int hitcount = 0;
	
	//-------------------- first pass --------------------
	//some error checking and creation of label tabel
	//vars hitcount, buffer, linecount;
	
	
	while(fgets(buffer, MAX, fptr)){
		
		char *ptr = strchr(buffer, '\n');
		check(ptr, "cannot handle linesize");
		
		ptr = buffer;
		while(*ptr){
			while(iswhitespace(ptr) == 0){
				
				
				//---------- handle letter hit ----------
				
				//if we hit comment go to end of file -> throwing rest of line
				if(*ptr == '#'){
					while(*ptr != '\n'){
						++ptr;
					}
					break;
				}
				
				//counting word hits -> more than 4 error
				//cannot error check every line
				if(hitcount > 2){
					check(0, "too many symbols");
				}
				++hitcount;
				
				//handle labels
				if(*ptr == '.'){
					linecount;
					char *ptr0 = ptr;
					while(iswhitespace(++ptr0) == 0)
						;
						
					char *name = malloc(ptr0 - ptr);
					check(name, "cannot allocate memory");
					
					int t = (ptr0 - 1) - ptr;
					name[t] = '\0';
					
					ptr0 -= 2;
					while(ptr0 >= ptr){
						name[ptr0 - ptr] = *(ptr0 + 1);
						--ptr0;
					}
					
					addtable(name, linecount * INSWIDTH);
					--linecount;
				}
				
				//------------------------------
				
				while(iswhitespace(ptr) == 0){
					++ptr;
				}
			}
			++ptr;
		}
		
		if(hitcount > 0){
			++linecount;
		}
		hitcount = 0;
	}
	
	//go to begin of file
	fseek(fptr, 0, SEEK_SET);
	//print_list();
	//putchar('\n');
	
	
	
	
	//-------------------- second pass --------------------
	
	char *opcodes[3];
	hitcount = 0;
	linecount = 0;
	
	while(fgets(buffer, MAX, fptr)){
		char *ptr = buffer;
		while(*ptr){
			while(iswhitespace(ptr) == 0){
				
				//---------- handle letter hit ----------
				
				//handle comments and labels
				if(*ptr == '#' || *ptr == '.'){
					while(*ptr != '\n'){
						++ptr;
					}
					break;
				}
				
				//save into token table
				opcodes[hitcount] = ptr;
				++hitcount;
				
				//------------------------------
				while(iswhitespace(ptr) == 0){
					++ptr;
				}
			}
			++ptr;
		}
		
		if(hitcount > 0){
			
			//---------- checking for execution ----------
			//printf("-%d %c %c %c-\n", linecount, *opcodes[0], *opcodes[1], *opcodes[2]);
			
			ptr = opcodes[0];
			//putchar(*ptr);
			
			int i;
			for(i = 0; i < OPCODES; ++i){
				int j = 0;
				ptr = opcodes[0];
				while(commands[i][j] != 0 && commands[i][j] == *ptr){
					++ptr;
					++j;
				}
				//printf("-%c %c- ", commands[i][j], *ptr);
				if(commands[i][j] == 0 && (iswhitespace(ptr) == 1 || *ptr == '\0')){
					
					//---------- execution ----------
					
					BYTE instruction[4];
					
					MWORD num;
					switch(i){
						case 0: //LD
							if(*opcodes[2] == 'R'){
								instruction[0] = 1;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}else if(*opcodes[2] == '$'){
								instruction[0] = 2;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2] + 1);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}else{			//load value
								instruction[0] = 0;
								instruction[1] = *(opcodes[1] + 1) - 48;
							
								num = getnum(opcodes[2]);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}
						case 1: //ST
							if(*opcodes[2] == '$'){
								instruction[0] = 0x10;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2] + 1);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}else if(*opcodes[2] == 'R'){
								instruction[0] = 1;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}
							break;
						case 2: //STL
							if(*opcodes[2] == '$'){
								instruction[0] = 0x11;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2] + 1);
								instruction[2] = *(((BYTE *)&num) + 1);
								instruction[3] = *(((BYTE *)&num));
							}else if(*opcodes[2] == 'R'){
								instruction[0] = 0x14;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = 0;
								instruction[3] = *(opcodes[2] + 1) - 48;
							}
							break;
						case 3: //STH
							if(*opcodes[2] == '$'){
								instruction[0] = 0x12;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2] + 1);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}else if(*opcodes[2] == 'R'){
								instruction[0] = 0x15;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}
							break;
						case 4: //CMP
							if(*opcodes[2] == 'R'){
								instruction[0] = 0x20;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}else{
								instruction[0] = 0x21;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2]);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}
							break;
						case 5: //BEQ
							instruction[0] = 0x30;
							instruction[1] = 0x00;
							
							num = getAddressByLabel(opcodes[1]);
							instruction[2] = *(((BYTE *)&num));
							instruction[3] = *(((BYTE *)&num) + 1);
							break;
						case 6: //BGT
							instruction[0] = 0x30;
							instruction[1] = 0x00;
							
							num = getAddressByLabel(opcodes[1]);
							instruction[2] = *(((BYTE *)&num));
							instruction[3] = *(((BYTE *)&num) + 1);
							break;
						case 7: //BLT
							instruction[0] = 0x32;
							instruction[1] = 0x00;
							
							num = getAddressByLabel(opcodes[1]);
							instruction[2] = *(((BYTE *)&num));
							instruction[3] = *(((BYTE *)&num) + 1);
							break;
						case 8: //BRA
							instruction[0] = 0x33;
							instruction[1] = 0x00;
							
							num = getAddressByLabel(opcodes[1]);
							instruction[2] = *(((BYTE *)&num));
							instruction[3] = *(((BYTE *)&num) + 1);
							break;
						case 9: //ADD
							if(*opcodes[2] == 'R'){
								instruction[0] = 0x42;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}else{
								instruction[0] = 0x40;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2]);
								//printf("0x%04x ", num);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}
							break;
						case 10: //SUB
							if(*opcodes[2] == 'R'){
								instruction[0] = 0x43;
								instruction[1] = *(opcodes[1] + 1) - 48;
								instruction[2] = *(opcodes[2] + 1) - 48;
								instruction[3] = 0;
							}else{
								instruction[0] = 0x41;
								instruction[1] = *(opcodes[1] + 1) - 48;
								
								num = getnum(opcodes[2] + 1);
								instruction[2] = *(((BYTE *)&num));
								instruction[3] = *(((BYTE *)&num) + 1);
							}
							break;
						case 11: //HALT
							instruction[0] = 0xfe;
							instruction[1] = 0xff;
							instruction[2] = 0xfe;
							instruction[3] = 0xff;
							break;
						case 12: //NOOP
						instruction[0] = 0xfe;
							instruction[0] = 0xff;
							instruction[1] = 0xff;
							instruction[2] = 0xff;
							instruction[3] = 0xff;
							break;
						
					}
					
					fwrite(instruction, 4, 1, out);
					//printf("0x%02x 0x%02x 0x%02x 0x%02x\n", instruction[0], instruction[1], instruction[2], instruction[3]);
					//--------------------
				}
			}
			//------------------------------
			
			++linecount;
			hitcount = 0;
		}
	}
	/**/
	destroytable();
	
	return 0;
}
