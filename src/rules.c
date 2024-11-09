#include <stdarg.h>
#include <stdio.h>
#include <string.h>


char tree[1024][128];
int tree_idx = 0;


void add_to_tree(char inpt[]){
	strcpy(tree[tree_idx++], inpt);
}

void attf(const char * frmt, ...){
	char tmp[1024];
	va_list arglist;
	va_start(arglist, frmt);
	vsprintf(tmp, frmt, arglist);
	va_end(arglist);

	strcpy(tree[tree_idx++], tmp);
}

int get_tree_len(void){ return tree_idx; }


char shift_addr[128] = { 0 };
int shift_type = 11;
int shifted = 0;


static int ncall = 0;

void get_label(char name[]){
	sprintf(name, ".L%d", ncall++);
}

