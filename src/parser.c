#include <string.h>
#include "types.h"
#include "helper.h"
#include "global.h"
#include "utility.h"


void parser(TKNS *tkns){
	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		// skip comment, white-space and newline
		if(tkns->tokens[tkns->idx].type == WHITESPACE ||
				tkns->tokens[tkns->idx].type == NEWLINE ||
				tkns->tokens[tkns->idx].type == COMMENT_TOK){ continue; }


		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			var_asgmt(tkns);

		// Check for #include "..." & #define ... ...
		} else if(tkns->tokens[tkns->idx].type == HASHTAG){
			macro_asgmt(tkns);

		// Check for 'for(...;...;...){...}'
		} else if(tkns->tokens[tkns->idx].type == FOR_KEWORD){
			for_asgmt(tkns);

		// Check for 'while(...){...}'
		} else if(tkns->tokens[tkns->idx].type == WHILE_KEWORD){
			body_asgmt(tkns, WHILE_BODY);

		// Check for 'if(...){...}'
		} else if(tkns->tokens[tkns->idx].type == IF_KEWORD){
			body_asgmt(tkns, IF_BODY);

		// Check for 'else{...}'
		} else if(tkns->tokens[tkns->idx].type == ELSE_KEWORD){
			else_asgmt(tkns);

		// Check for 'return ...;'
		} else if(tkns->tokens[tkns->idx].type == RETURN_KEWORD){
			return_asgmt(tkns);

		} else {
			throw_err(tkns, "Invalid word", NULL);
			exit(0);
		}
	}
}

