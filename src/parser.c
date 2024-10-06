#include <string.h>
#include "types.h"
#include "helper.h"


void parser(TKNS *tkns){
	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		if(tkns->tokens[tkns->idx].type == WHITESPACE || tkns->tokens[tkns->idx].type == NEWLINE){ continue; }

		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			var_asgmt(tkns);
		} else if(tkns->tokens[tkns->idx].type == HASHTAG){
			macro_asgmt(tkns);
		} else if(tkns->tokens[tkns->idx].type == FOR_KEWORD){
			for_asgmt(tkns);
		} else if(tkns->tokens[tkns->idx].type == WHILE_KEWORD){
			body_asgmt(tkns, WHILE_BODY);
		} else if(tkns->tokens[tkns->idx].type == IF_KEWORD){
			body_asgmt(tkns, IF_BODY);
		} else if(tkns->tokens[tkns->idx].type == ELSE_KEWORD){
			else_asgmt(tkns);
		} else if(tkns->tokens[tkns->idx].type == RETURN_KEWORD){
			return_asgmt(tkns);
		} else {
		}
	}
}

