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
	char tmp[1024] = { 0 };
	va_list arglist;
	va_start(arglist, frmt);
	vsprintf(tmp, frmt, arglist);
	va_end(arglist);
	strcpy(tree[tree_idx++], tmp);
}

int tree_len(void){ return tree_idx; }


/* Label */
static int ncall = 0;
static char label_buffer[256][20];

const char *get_label(void){
	snprintf(label_buffer[ncall], sizeof(label_buffer), ".L%d", ncall);
	ncall++;
	return label_buffer[ncall - 1];
}

void get_label_buff(char name[]){
	sprintf(name, ".L%d", ncall++);
}


void clear_labels(void){
	ncall = 0;
	memset(label_buffer, 0, sizeof(label_buffer));
}

