#include <stdio.h>
#include <string.h>
#include "types.h"
#include "helper.h"
#include "global.h"
#include "utility.h"

static var_t return_type = INT_VAR;

void parser(TKNS *tkns){

	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		// skip comment, white-space and newline
		if(tkns->tokens[tkns->idx].type == WHITESPACE ||
				tkns->tokens[tkns->idx].type == NEWLINE ||
				tkns->tokens[tkns->idx].type == COMMENT_TOK){ continue; }


		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			ASGMT asgmt = var_asgmt(tkns);
			printf("%s:\n", asgmt.name);

			if(asgmt.is_func){
				return_type = asgmt.func.return_type;
				parser(&asgmt.func.body);
				save_func_global(asgmt.func);

			} else {
				VAR v;
				v.value = 0;
				v.type = asgmt.type;
				strcpy(v.name, asgmt.name);
				if(asgmt.type == INT_VAR || asgmt.type == CHAR_VAR){
					v.value = asgmt.value;
				} else {
					strcpy(v.str_value, asgmt.str);
				}
				save_global_variable(v);
			}

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
			CNST_VAR rtrn = return_asgmt(tkns, return_type);
			printf("    RETLW 0x%x\n", rtrn.type == INT_VAR ? rtrn.int_value : rtrn.char_value);

		} else {
			throw_err(tkns, "Invalid word", NULL);
			exit(0);
		}
	}
}

