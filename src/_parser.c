#include <stdio.h>
#include <string.h>
#include "types.h"
#include "helper.h"
#include "global.h"
#include "utility.h"
#include "emission.h"

static var_t return_type = INT_VAR;

void parser(TKNS *tkns, int allow_expression, int *tidx, ast_t refer){
	AST ast;

	int if_detected = 0;

	ast.type = AST_NO_STATEMENT;
	ast.func.arg_len = 0;

	ast.asgmt.value = 0;
	ast.asgmt.is_str = 0;
	ast.asgmt.is_func = 0;

	ast.value.int_value = 0;

	ast.expr.is_call = 0;
	ast.expr.is_assign = 0;
	ast.expr.mono_side = 0;


	ast.refer = refer;


	ast.left = NULL;
	ast.right = NULL;
	ast.next = NULL;

	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){

		TKNS body;
		body.idx = 0;
		body.max = 0;

		// skip comment, whitespace and newline
		if(tkns->tokens[tkns->idx].type == WHITESPACE ||
				tkns->tokens[tkns->idx].type == NEWLINE ||
				tkns->tokens[tkns->idx].type == COMMENT_TOK){ continue; }


		// Check for varialbe assignments
		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 || strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
			ASGMT asgmt = var_asgmt(tkns);

			if(asgmt.is_func){
				return_type = asgmt.func.return_type;
				// parser(&asgmt.func.body, 1, tidx);
				body = asgmt.func.body;
				save_func_global(asgmt.func);
				ast.func = asgmt.func;
				ast.type = AST_FUNCTION_ASSIGNMENT;

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
				ast.type = AST_VARIABLE_ASSIGNMENT;
			}

			ast.asgmt = asgmt;

		// Check for #include "..." & #define ... ...
		} else if(tkns->tokens[tkns->idx].type == HASHTAG){
			macro_asgmt(tkns);

		// Check for 'for(...;...;...){...}'
		} else if(tkns->tokens[tkns->idx].type == FOR_KEYWORD){
			FOR_ASGMT fa = for_asgmt(tkns);
			body = fa.body;
			ast.type = AST_FOR_LOOP_ASSIGNMENT;

		// Check for 'while(...){...}'
		} else if(tkns->tokens[tkns->idx].type == WHILE_KEYWORD){
			BODY_ASGMT ba = body_asgmt(tkns, WHILE_BODY);
			body = ba.body;
			ast.type = AST_WHILE_LOOP_ASSIGNMENT;

		} else if(tkns->tokens[tkns->idx].type == BACKTICK_SIGN){
			tkns->idx++;
			char raw_asm[128] = { 0 };
			while(tkns->tokens[tkns->idx].type != BACKTICK_SIGN){
				strcat(raw_asm, tkns->tokens[tkns->idx].word);
				if(tkns->tokens[tkns->idx].type == NEWLINE){
					throw_err(tkns, "syntax error", "`");
				}
				tkns->idx++;
			}
			tkns->idx++;

			skip_white_space(tkns);
			pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
			strcpy(ast.raw_asm, raw_asm);
			ast.type = AST_RAW_ASM;


		// Check for 'if(...){...}'
		} else if(tkns->tokens[tkns->idx].type == IF_KEYWORD){
			BODY_ASGMT ba = body_asgmt(tkns, IF_BODY);
			body = ba.body;
			ast.cond = ba.cond;
			ast.type = AST_IF_STATEMENT;
			refer = AST_IF_STATEMENT;
			if_detected = 1;

		// Check for 'else {...}'
		} else if(tkns->tokens[tkns->idx].type == ELSE_KEYWORD && if_detected){
			BODY_ASGMT ba = else_asgmt(tkns);
			body = ba.body;
			ast.type = AST_ELSE_STATEMENT;
			refer = AST_ELSE_STATEMENT;

		// Check for 'return ...;'
		} else if(tkns->tokens[tkns->idx].type == RETURN_KEYWORD){
			CNST_VAR rtrn = return_asgmt(tkns, return_type);
			ast.type = AST_RETURN_STATEMENT;
			ast.value = rtrn;

		} else {
			if(allow_expression){
				EXPR xpr = get_expr(tkns, END_SIGN);
				ast.expr = xpr;
				ast.type = AST_STATEMENT;
				if(ast.expr.is_call){
					ast.type = AST_FUNCTION_CALL;
					ast.func = xpr.caller;
				}

			} else {
				throw_err(tkns, "Invalid word", NULL);
			}
		}


		if(if_detected && ast.type != AST_IF_STATEMENT){ if_detected = 0; }


		show_ast_info(ast);

		if(ast.type != AST_NO_STATEMENT){
			char code[100];
			memset(code, '\0', sizeof(code));
			ast.refer = refer;

			code_emission(ast, code);
			update_tree_lines(tidx, code);
		}


		if(body.max != 0){
			parser(&body, 1, tidx, refer);
			ast.refer = AST_NO_STATEMENT;
		}
	}
	

}

