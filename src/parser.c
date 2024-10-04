#include <string.h>
#include "types.h"
#include "helper.h"


void parser(TKNS *tkns){
	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		if(tkns->tokens[tkns->idx].type == WHITESPACE || tkns->tokens[tkns->idx].type == NEWLINE){ continue; }

		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			var_asgmt(tkns);
		} else {
		}
	}
}

