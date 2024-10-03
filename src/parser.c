#include <stdio.h>
#include <string.h>
#include "types.h"
#include "helper.h"


void gotoxy(int x, int y){ 
	printf("\033[%d;%df", y, x); 
}


EXPR check_assignment(TOKEN *tokens, int max, int cidx){
	return VARIABLE_ASSIGNEMNT;
}


void parser(TOKEN *tokens, int tlen){
	EXPR expr;
	for(int i = 0; i < tlen; ++i){

		if(tokens[i].type == WHITESPACE || tokens[i].type == NEWLINE){ continue; }

		if(strcmp(tokens[i].word, "int") == 0){
			if(check_assignment(tokens, tlen, i) == VARIABLE_ASSIGNEMNT){
				var_asgmt(tokens, tlen, &i);
			}
		}
	}
}

