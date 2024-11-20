#include <stdarg.h>
#include <stdio.h>
#include <string.h>


char tree[1024][128] = { 0 };
int tree_idx = 0;


// void add_to_tree(char inpt[]){
// 	strcpy(tree[tree_idx++], inpt);
// }

/* add to tree (formated) */
void attf(const char * frmt, ...){
	char tmp[1024];
	va_list arglist;
	va_start(arglist, frmt);
	vsprintf(tmp, frmt, arglist);
	va_end(arglist);
	strcpy(tree[tree_idx++], tmp);
}

int tree_len(void){ return tree_idx; }


/* Label */
static int ncall = 0;
static char buffer[256][20];

const char *get_label(void){
	snprintf(buffer[ncall], sizeof(buffer), ".L%d", ncall);
	ncall++;
	return buffer[ncall - 1];
}

void get_label_buff(char name[]){
	sprintf(name, ".L%d", ncall++);
}

