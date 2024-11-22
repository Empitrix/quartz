#include "types.h"
#include "helper.h"
#include <stdio.h>

static int indent = 0;

void qparser(TKNS *tkns, int allow_expr, ast_t refer){
	if(empty_body(tkns)){ return; }

	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){
		TKNS *body = NULL;
		if(skip_empty_tok(tkns->tokens[tkns->idx])){ continue; }

		Qast *ast = empty_ast();

		if(tkns->tokens[tkns->idx].type == INT_KEYWORD || tkns->tokens[tkns->idx].type == CHAR_KEYWORD || tkns->tokens[tkns->idx].type == HASHTAG){
			get_snippet(tkns, END_SIGN, &ast->snip);

			if(ast->snip.assigne_type == FUNCTION_ASSIGNMENT_ASG){
				body = &ast->snip.func.body;
				ast->type = AST_FUNCTION_ASSIGNMENT;

			} else {
				ast->type = AST_VARIABLE_ASSIGNMENT;
			}


		} else if (tkns->tokens[tkns->idx].type == FOR_KEYWORD){
			for_asgmt(tkns, &ast->qfor);
			ast->type = AST_FOR_LOOP_ASSIGNMENT;
			body = &ast->qfor.body;

		} else if (tkns->tokens[tkns->idx].type == IF_KEYWORD){
			ast->qif = if_asgmt(tkns);
			ast->type = AST_IF_STATEMENT;
			body = &ast->qif.if_body;

		} else if (tkns->tokens[tkns->idx].type == WHILE_KEYWORD){
			ast->qwhile = while_asgmt(tkns);
			ast->type = AST_WHILE_LOOP_ASSIGNMENT;
			body = &ast->qwhile.body;

		} else if (tkns->tokens[tkns->idx].type == RETURN_KEYWORD){
			handle_return(tkns, &ast->var);
			ast->type = AST_RETURN_STATEMENT;

		} else if (tkns->tokens[tkns->idx].type == BACKTICK_SIGN){
			handle_rasm(tkns, ast->rasm);
			ast->type = AST_RAW_ASM;

		} else {
			if(allow_expr){

				get_snippet(tkns, END_SIGN, &ast->snip);

				if(ast->snip.type == FUNCTION_CALL_SNIP){
					ast->type = AST_FUNCTION_CALL;
				} else {
					ast->type = AST_STATEMENT;
				}



			} else {
				throw_err(tkns, "Invalid word", NULL);
			}
		}

		ast->depth = indent;
		ast->refer = refer;


		if(body != NULL && body->max != 0){
			indent++;
			qparser(body, 1, ast->type);
			indent--;

			if(ast->type == AST_IF_STATEMENT && ast->qif.contains_else){
				// create a dummy "else"
				Qast *a = empty_ast();
				a->depth = indent;
				a->type = AST_ELSE_STATEMENT;

				// capture "else" body
				indent++;
				qparser(&ast->qif.else_body, 1, AST_ELSE_STATEMENT);
				indent--;
			}
		}

	}


	// ast->indent = gindent;
}



// void parser(TKNS *tkns, int allow_expr, ast_t refer){
// 	AST ast;
// 
// 	int if_detected = 0;
// 
// 	ast.type = AST_NO_STATEMENT;
// 	ast.func.arg_len = 0;
// 
// 	ast.asgmt.value = 0;
// 	ast.asgmt.is_str = 0;
// 	ast.asgmt.is_func = 0;
// 
// 	ast.value.int_value = 0;
// 
// 	ast.expr.type = EXPR_EMPTY;
// 	ast.expr.mono_side = 0;
// 
// 
// 	ast.refer = refer;
// 
// 
// 	ast.cond.op = NO_OP;
// 	ast.cond.literal = 0;
// 
// 	ast.for_asgmt.cond.op = NO_OP;
// 	ast.for_asgmt.cond.literal = 0;
// 	
// 	ast.init.op = NO_OP;
// 	ast.init.literal = 0;
// 
// 	ast.macro.value.type = INT_VAR;
// 	ast.macro.type = DEFINE_MACRO;
// 	ast.macro.value.int_value = 0;
// 	ast.macro.value.char_value = 0;
// 
// 	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){
// 
// 		TKNS body;
// 		body.idx = 0;
// 		body.max = 0;
// 
// 		// skip comment, whitespace and newline
// 		if(tkns->tokens[tkns->idx].type == WHITESPACE ||
// 				tkns->tokens[tkns->idx].type == NEWLINE ||
// 				tkns->tokens[tkns->idx].type == COMMENT_TOK){ continue; }
// 
// 
// 		// Check for varialbe assignments
// 		if(tkns->tokens[tkns->idx].type == INT_KEYWORD || tkns->tokens[tkns->idx].type == CHAR_KEYWORD || tkns->tokens[tkns->idx].type == HASHTAG){
// 			get_snippet(tkns, END_SIGN);
// 
// 		// Check for 'for(...;...;...){...}'
// 		} else if(tkns->tokens[tkns->idx].type == FOR_KEYWORD){
// 			for_asgmt(tkns);
// 
// 		// Check for 'while(...){...}'
// 		} else if(tkns->tokens[tkns->idx].type == WHILE_KEYWORD){
// 			while_asgmt(tkns);
// 
// 
// 		} else if(tkns->tokens[tkns->idx].type == BACKTICK_SIGN){
// 			// tkns->idx++;
// 			// char raw_asm[128] = { 0 };
// 			// int island = 0;
// 			// int island_open = 0;
// 			// while(tkns->tokens[tkns->idx].type != BACKTICK_SIGN){
// 
// 			// 	if(tkns->tokens[tkns->idx].type == BRACE_OPN){
// 			// 		tkns->idx++;
// 			// 		island++;
// 			// 		island_open = 1;
// 			// 		continue;
// 			// 	}
// 
// 			// 	if(tkns->tokens[tkns->idx].type == BRACE_CLS){
// 			// 		tkns->idx++;
// 			// 		island--;
// 			// 		island_open = 0;
// 			// 		continue;
// 			// 	}
// 
// 			// 	int isave = tkns->idx; 
// 			// 	if(tkns->tokens[tkns->idx].type == IDENTIFIER && island_open == 1){
// 			// 		CNST_VAR var = get_value(tkns);
// 			// 		ARG arg = get_arg_struct(refer_func, var.name);
// 
// 			// 		if(strcmp(arg.name, "") == 0){
// 			// 			VAR v;
// 			// 			get_variable(tkns->tokens[isave].word, GLOBAL_TARGET, &v);
// 			// 			if(v.type == MACRO_VAR){
// 			// 				strcpy(arg.name, v.name);
// 			// 			}
// 			// 		}
// 
// 			// 		
// 			// 		strcatf(raw_asm, "%s", arg.name);
// 			// 		continue;
// 			// 	}
// 
// 
// 			// 	strcat(raw_asm, tkns->tokens[tkns->idx].word);
// 			// 	if(tkns->tokens[tkns->idx].type == NEWLINE){
// 			// 		throw_err(tkns, "syntax error", "`");
// 			// 	}
// 			// 	tkns->idx++;
// 			// }
// 			// tkns->idx++;
// 
// 			// skip_whitespace(tkns);
// 			// pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
// 			// strcpy(ast.raw_asm, raw_asm);
// 			// ast.type = AST_RAW_ASM;
// 
// 
// 		// Check for 'if(...){...}'
// 		} else if(tkns->tokens[tkns->idx].type == IF_KEYWORD){
// 			if_asgmt(tkns);
// 
// 		// Check for 'return ...;'
// 		} else if(tkns->tokens[tkns->idx].type == RETURN_KEYWORD){
// 			CNST_VAR rtrn = return_asgmt(tkns, return_type);
// 			ast.type = AST_RETURN_STATEMENT;
// 			ast.value = rtrn;
// 			main_refer = AST_NO_STATEMENT;
// 
// 		} else {
// 			if(allow_expr){
// 				get_snippet(tkns, END_SIGN);
// 
// 			} else {
// 				throw_err(tkns, "Invalid word", NULL);
// 			}
// 		}
// 
// 
// 		if(if_detected && ast.type != AST_IF_STATEMENT){ if_detected = 0; }
// 
// 
// 		if(ast.type != AST_NO_STATEMENT){
// 			char code[100] = { 0 };
// 			memset(code, '\0', sizeof(code));
// 
// 			add_ast(ast);
// 			ast.refer = refer;
// 		}
// 
// 
// 		if(body.max != 0){
// 			parser(&body, 1, refer);
// 			// refer = AST_NO_STATEMENT;
// 
// 			refer = main_refer;
// 			// refer = pop_refers();
// 			ast.refer = refer;
// 		}
// 	}
// }
