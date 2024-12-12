#include "types.h"
#include "helper.h"
#include <stdio.h>

static int indent = 0;

void qparser(TKNS *tkns, int allow_expr, ast_t refer){
	clr_err_buff();
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
			while_asgmt(tkns, &ast->qwhile);
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
}


void update_children(){
	int i = 0;
	int j = 0;
	for(i = 0; i < qast_idx; ++i){
		for(j = 1; j < qast_idx; ++j){
			int save = qasts[i].children;
			if(qasts[i].depth < qasts[i + j].depth){
				qasts[i].children++;
			}
			if(save == qasts[i].children){ break; }
		}
	}
}

