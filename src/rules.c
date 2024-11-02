#include <stdio.h>
#include <string.h>


char tree[1000][100];
int tree_idx = 0;
int insts = 0;  // number of instructions (used for storing path ...)

char shift_addr[128] = { 0 };
int shift_type = 11;
int shifted = 0;


static int ncall = 0;

void get_unique_lable(const char suggest[], char name[]){
	if(suggest == NULL || strcmp(suggest, "") == 0){
		suggest = "handler";
	}
	sprintf(name, "%s%d:", suggest, ncall++);
}


