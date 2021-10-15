#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 256
#define INSWIDTH 4

struct _mark{
	int address;
	char *name;
	struct _mark *next;
};
typedef struct _mark mark;

int iswhitespace(char *c){
	return (*c == ' ' || *c == '\n' || *c == '\t');
}

mark *table;

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

int main(){
	table = NULL;
	
	FILE *fptr = fopen("test.asm", "r");
	check(fptr, "cannot allocate memory");
	
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
	print_list();
	putchar('\n');
	
	//-------------------- second pass --------------------
	
	
	char *opcodes[3];
	hitcount = 0;
	linecount = 0;
	
	while(fgets(buffer, MAX, fptr)){
		char *ptr = buffer;
		while(*ptr){
			while(iswhitespace(ptr) == 0){
				//---------- handle letter hit ----------
				
				if(*ptr == '.'){	//label
					while(iswhitespace(ptr) == 0){
						++ptr;
					}
					--linecount;
				}
				
				if(*ptr == '#'){	//comment
					while(*ptr != '\n'){
						++ptr;
					}
					break;
				}
				
				//save into token table
				opcodes[hitcount] = ptr;
				++hitcount;
				
				//
				while(iswhitespace(ptr) == 0){
					++ptr;
				}
			}
			++ptr;
		}
		
		//printf("%d\n", linecount);
		printf("-%d %c %c %c-\n", linecount, *opcodes[0], *opcodes[1], *opcodes[2]);
		if(hitcount > 0){
			++linecount;
			hitcount = 0;
		}
	}
	/**/
	
	return 0;
}
