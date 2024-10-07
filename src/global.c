#include "types.h"
#include <stdio.h>

#define MAX_VAR_SZIE 100

static VAR global_var[MAX_VAR_SZIE];
static int gvar_idx = 0;

static VAR scoop_var[MAX_VAR_SZIE];
static int svar_idx = 0;


void save_global_variable(VAR v){
	global_var[gvar_idx++] = v;
}



void show_global_variables(void){
	printf("GLOBAL VARIABLES:\n");
	for(int i = 0; i < gvar_idx; ++i){
		printf("  %d. %s: ", i + 1, global_var[i].name);
		switch (global_var[i].type) {
			case INT_VAR:
				printf("%d\n", global_var[i].value);
				break;

			case CHAR_VAR:
				printf("\'%c\'\n", global_var[i].value);
				break;

			case STR_VAR:
				printf("\"%s\"\n", global_var[i].str_value);
				break;
		}
	}
	putchar('\n');
}
