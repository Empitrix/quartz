#include <stdio.h>
#include <string.h>
#include "types.h"
#include "helper.h"
#include "global.h"
#include "utility.h"

static var_t return_type = INT_VAR;

void parser(TKNS *tkns, int allow_expression){

	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		// skip comment, white-space and newline
		if(tkns->tokens[tkns->idx].type == WHITESPACE ||
				tkns->tokens[tkns->idx].type == NEWLINE ||
				tkns->tokens[tkns->idx].type == COMMENT_TOK){ continue; }


		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			ASGMT asgmt = var_asgmt(tkns);

			if(asgmt.is_func){
				return_type = asgmt.func.return_type;
				parser(&asgmt.func.body, 1);
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
		} else if(tkns->tokens[tkns->idx].type == FOR_KEYWORD){
			FOR_ASGMT fa = for_asgmt(tkns);
			parser(&fa.body, 1);

		// Check for 'while(...){...}'
		} else if(tkns->tokens[tkns->idx].type == WHILE_KEYWORD){
			BODY_ASGMT ba = body_asgmt(tkns, WHILE_BODY);
			parser(&ba.body, 1);

		// Check for 'if(...){...}'
		} else if(tkns->tokens[tkns->idx].type == IF_KEYWORD){
			BODY_ASGMT ba = body_asgmt(tkns, IF_BODY);
			parser(&ba.body, 1);

		// Check for 'else{...}'
		} else if(tkns->tokens[tkns->idx].type == ELSE_KEYWORD){
			BODY_ASGMT ba = else_asgmt(tkns);
			parser(&ba.body, 1);

		// Check for 'return ...;'
		} else if(tkns->tokens[tkns->idx].type == RETURN_KEYWORD){
			CNST_VAR rtrn = return_asgmt(tkns, return_type);

		} else {
			if(allow_expression){
				EXPR xpr = get_expr(tkns, END_SIGN);

			} else {
				throw_err(tkns, "Invalid word", NULL);
				exit(0);
			}
		}
	}
}

