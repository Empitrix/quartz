#include <stdio.h>
#include <string.h>
#include "types.h"
#include "helper.h"
#include "global.h"
#include "utility.h"
#include "emission.h"

static var_t return_type = INT_VAR;

static ast_t main_refer = AST_NO_STATEMENT;
func_t refer_func;


static ast_t refers[20] = { AST_NO_STATEMENT };
static int refers_idx = 0;

/* STACK */
void push_refers(ast_t value){
	refers[refers_idx++] = value;
	main_refer = value;
	// printf("LEN: %d\n", refers_idx);
}
ast_t pop_refers(void){
	ast_t popped = refers[0];
	for(int i = 1; i < refers_idx; ++i){
		refers[i - 1] = refers[i];
	}

	refers[refers_idx] = AST_NO_STATEMENT;
	// printf("LEN: %d\n", refers_idx);
	return popped;
}


void parser(TKNS *tkns, int allow_expression, int *tidx, ast_t refer){
	AST ast;

	// refer_func.arg_len = 0;
	// refer_func.return_type = INT_VAR;

	int if_detected = 0;

	ast.type = AST_NO_STATEMENT;
	ast.func.arg_len = 0;

	ast.asgmt.value = 0;
	ast.asgmt.is_str = 0;
	ast.asgmt.is_func = 0;

	ast.value.int_value = 0;

	ast.expr.type = EXPR_EMPTY;
	ast.expr.mono_side = 0;


	ast.refer = refer;

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
			get_snippet(tkns);
			
			// ASGMT asgmt = var_asgmt(tkns);

			// if(asgmt.is_func){
			// 	return_type = asgmt.func.return_type;
			// 	// parser(&asgmt.func.body, 1, tidx);
			// 	body = asgmt.func.body;
			// 	save_func_global(asgmt.func);
			// 	ast.func = asgmt.func;
			// 	ast.type = AST_FUNCTION_ASSIGNMENT;
			// 	refer = AST_FUNCTION_ASSIGNMENT;
			// 	main_refer = AST_FUNCTION_ASSIGNMENT;
			// 	// push_refers(AST_FUNCTION_ASSIGNMENT);
			// 	refer_func = ast.func;

			// } else {
			// 	VAR v;
			// 	v.value = 0;
			// 	v.type = asgmt.type;
			// 	strcpy(v.name, asgmt.name);
			// 	if(asgmt.type == INT_VAR || asgmt.type == CHAR_VAR){
			// 		v.value = asgmt.value;
			// 	} else {
			// 		strcpy(v.str_value, asgmt.str);
			// 	}
			// 	if(refer == AST_NO_STATEMENT){
			// 		save_global_variable(v);
			// 	} else {
			// 		save_scoop_variable(v);
			// 	}

			// 	asgmt.address = pop_ram();
			// 	ASM_VAR avar;
			// 	strcpy(avar.name, v.name);
			// 	avar.addr = asgmt.address;
			// 	add_asm_var(avar);

			// 	// ast.asgmt.name
			// 	ast.type = AST_VARIABLE_ASSIGNMENT;
			// }

			// ast.asgmt = asgmt;

		// Check for #include "..." & #define ... ...
		} else if(tkns->tokens[tkns->idx].type == HASHTAG){
			ast.macro = macro_asgmt(tkns);
			ast.type = AST_MACRO;

			VAR v;
			v.type = INT_VAR;
			strcpy(v.name, ast.macro.name);
			v.value = ast.macro.value.int_value;
			v.type = MACRO_VAR;
			save_global_variable(v);


			// asgmt.address = pop_ram();
			ASM_VAR avar;
			strcpy(avar.name, v.name);
			avar.addr = ast.macro.value.int_value;
			add_asm_var(avar);


		// Check for 'for(...;...;...){...}'
		} else if(tkns->tokens[tkns->idx].type == FOR_KEYWORD){
			FOR_ASGMT fa = for_asgmt(tkns);
			body = fa.body;
			ast.type = AST_FOR_LOOP_ASSIGNMENT;
			ast.for_asgmt = fa;
			// ast.init = fa.init;
			// ast.cond = fa.cond;

			refer = AST_FOR_LOOP_ASSIGNMENT;

			// main_refer = AST_FOR_LOOP_ASSIGNMENT;
			// push_refers(AST_FOR_LOOP_ASSIGNMENT);

		// Check for 'while(...){...}'
		} else if(tkns->tokens[tkns->idx].type == WHILE_KEYWORD){
			BODY_ASGMT ba = body_asgmt(tkns, WHILE_BODY);
			body = ba.body;
			ast.cond = ba.cond;
			ast.type = AST_WHILE_LOOP_ASSIGNMENT;
			refer = AST_WHILE_LOOP_ASSIGNMENT;

		} else if(tkns->tokens[tkns->idx].type == BACKTICK_SIGN){
			tkns->idx++;
			char raw_asm[128] = { 0 };
			int island = 0;
			int island_open = 0;
			while(tkns->tokens[tkns->idx].type != BACKTICK_SIGN){

				if(tkns->tokens[tkns->idx].type == BRACE_OPN){
					tkns->idx++;
					island++;
					island_open = 1;
					continue;
				}

				if(tkns->tokens[tkns->idx].type == BRACE_CLS){
					tkns->idx++;
					island--;
					island_open = 0;
					continue;
				}

				int isave = tkns->idx; 
				if(tkns->tokens[tkns->idx].type == IDENTIFIER && island_open == 1){
					CNST_VAR var = get_value(tkns);
					int addr;
					ARG arg = get_arg_struct(refer_func, var.name);
					addr = arg.addr;

					// printf("(RAW ASM): {name: %s, addr: %d}\n", arg.name, arg.addr);
					// printf("(IDENTIFIER): %s\n", tkns->tokens[isave].word);

					if(strcmp(arg.name, "") == 0){
						VAR v;
						get_variable(tkns->tokens[isave].word, GLOBAL_TARGET, &v);
						if(v.type == MACRO_VAR){
							strcpy(arg.name, v.name);
						}
					}

					
					strcatf(raw_asm, "%s", arg.name);
					continue;
				}


				strcat(raw_asm, tkns->tokens[tkns->idx].word);
				if(tkns->tokens[tkns->idx].type == NEWLINE){
					throw_err(tkns, "syntax error", "`");
				}
				tkns->idx++;
			}
			tkns->idx++;

			skip_whitespace(tkns);
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
			// refer = AST_NO_STATEMENT;
			main_refer = AST_NO_STATEMENT;
			// push_refers(AST_NO_STATEMENT);

		} else {
			if(allow_expression){
				EXPR xpr = get_expr(tkns, END_SIGN);
				ast.expr = xpr;
				ast.type = AST_STATEMENT;
				if(ast.expr.type == EXPR_FUNCTION_CALL){
					ast.type = AST_FUNCTION_CALL;
					ast.func = xpr.caller;
				}

			} else {
				throw_err(tkns, "Invalid word", NULL);
			}
		}


		if(if_detected && ast.type != AST_IF_STATEMENT){ if_detected = 0; }


		// show_ast_info(ast);

		if(ast.type != AST_NO_STATEMENT){
			char code[100] = { 0 };
			memset(code, '\0', sizeof(code));

			add_ast(ast);
			ast.refer = refer;


			// code_emission(ast, code);
			// update_tree_lines(tidx, code);
		}


		if(body.max != 0){
			parser(&body, 1, tidx, refer);
			// refer = AST_NO_STATEMENT;


			refer = main_refer;
			// refer = pop_refers();
			ast.refer = refer;
		}


	}
	

}

