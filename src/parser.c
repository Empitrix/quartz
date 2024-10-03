#include <string.h>
#include "types.h"
#include "helper.h"


void parser(TOKEN *tokens, int tlen){
	for(int i = 0; i < tlen; ++i){

		if(tokens[i].type == WHITESPACE || tokens[i].type == NEWLINE){ continue; }

		if(strcmp(tokens[i].word, "int") == 0 || strcmp(tokens[i].word, "char") == 0){
			var_asgmt(tokens, tlen, &i);
		} else {
		}
	}
}

