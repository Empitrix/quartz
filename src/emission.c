#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "rules.h"
#include "utility.h"
#include "global.h"
#include "helper.h"

// RAM address
static int ram_stack[MAX_RAM + 1] = {0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00};


/* pop_ram: pop one ram address (if address is 0 it means that this is an invalid address)*/
int pop_ram(){
	int out = ram_stack[0];
	for(int i = 0; i < MAX_RAM - 1; ++i){
		ram_stack[i] = ram_stack[i + 1];
	}
	if(out == 0){
		printf("Too many variable definition (out of ram)\n");
		exit(0);
	}
	return out;
}

static int main_found = 0;


static char assigned_args[MAX_ARG][NAME_MAX] = { 0 };
static int aa_idx = 0;

int arg_name_exists(char name[]){
	for(int i = 0; i < aa_idx; ++i){
		if(strcmp(assigned_args[i], name) == 0){
			return 1;
		}
	}
	return 0;
}

void assign_arg(char buff[], ARG arg){
	if(arg_name_exists(arg.name)){ return; }
	strcatf(buff, "\t%s EQU 0x%x\n", arg.name, pop_ram());
	strcpy(assigned_args[aa_idx++], arg.name);
}


void code_emission(AST ast, char code[], char label[]){

	switch(ast.type){
		case AST_VARIABLE_ASSIGNMENT:
			if(ast.asgmt.is_func == 0 && ast.asgmt.is_str == 0){
				if(ast.asgmt.type == INT_VAR || ast.asgmt.type == CHAR_VAR){
					strcatf(code, "%s EQU 0x%x\nMOVLW 0x%x\nMOVWF %s",
						ast.asgmt.name, ast.asgmt.address, ast.asgmt.value, ast.asgmt.name);
					// *length = 2;
				}
			}
			break;


		case AST_WHILE_LOOP_ASSIGNMENT:
			if(ast.cond.op == EQUAL_OP || ast.cond.op == NOT_EQUAL_OP){  // ==
				strcatf(code, "\tMOVF %s, 0\n", ast.cond.left); 
				if(ast.cond.literal){
					strcatf(code, "\tXORLW %s\n", ast.cond.right);
				} else {
					strcatf(code, "\tXORWF %s, 0\n", ast.cond.right);
				}
				if(ast.cond.op == NOT_EQUAL_OP){
					strcatf(code, "\tBTFSC STATUS, Z");
				} else {
					strcatf(code, "\tBTFSS STATUS, Z");
				}
			}
			break;

		case AST_FOR_LOOP_ASSIGNMENT:

			strcatf(code, "\tMOVLW %s\n\tMOVWF %s\n", ast.for_asgmt.init.right, ast.for_asgmt.init.left);
			get_label_buff(label);
			strcatf(code, "%s:\n", label);

			// if(ast.for_asgmt.cond.op == EQUAL_OP || ast.for_asgmt.cond.op == NOT_EQUAL_OP){  // ==
			// 	strcatf(code, "\tMOVF %s, 0\n", ast.for_asgmt.cond.left); 
			// 	if(ast.for_asgmt.cond.literal){
			// 		strcatf(code, "\tXORLW %s\n", ast.for_asgmt.cond.right);
			// 	} else {
			// 		strcatf(code, "\tXORWF %s, 0\n", ast.for_asgmt.cond.right);
			// 	}
			// 	if(ast.for_asgmt.cond.op == NOT_EQUAL_OP){
			// 		strcatf(code, "\tBTFSS STATUS, Z");
			// 	} else {
			// 		strcatf(code, "\tBTFSC STATUS, Z");
			// 	}
			// }
			emit_stmt(code, ast.for_asgmt.cond, 1);
			break;


		case AST_IF_STATEMENT:
			// if(ast.cond.op == EQUAL_OP || ast.cond.op == NOT_EQUAL_OP){  // ==
			// 	strcatf(code, "\tMOVF %s, 0\n", ast.cond.left); 
			// 	if(ast.cond.literal){
			// 		strcatf(code, "\tXORLW %s\n", ast.cond.right);
			// 	} else {
			// 		strcatf(code, "\tXORWF %s, 0\n", ast.cond.right);
			// 	}
			// 	if(ast.cond.op == NOT_EQUAL_OP){
			// 		strcatf(code, "\tBTFSC STATUS, Z");
			// 	} else {
			// 		strcatf(code, "\tBTFSS STATUS, Z");
			// 	}
			// }

			emit_stmt(code, ast.for_asgmt.cond, 0);
			break;

		case AST_ELSE_STATEMENT:
			break;

		case AST_MACRO:
			sprintf(code, "%s EQU 0x%x", ast.macro.name, ast.macro.value.int_value);
			break;

		case AST_FUNCTION_ASSIGNMENT:
			if(strcmp(ast.func.name, "main") == 0 && main_found == 0){ main_found = 1; }
			sprintf(code, "%s:", ast.func.name);
			break;

		case AST_RETURN_STATEMENT:
			if(main_found){
				main_found = -1;
				sprintf(code, "\tSLEEP  ; End of the 'main'");
			} else {
				sprintf(code, "\tRETLW %d", ast.value.int_value);
			}

			// *length = 1;
			break;

		case AST_FUNCTION_CALL:
			for(int i = 0; i < ast.func.arg_len; ++i){

				assign_arg(code, ast.func.args[i]);

				// Pass variable
				if(strcmp(ast.expr.args[i].name, "") != 0){
					strcatf(code, "\tMOVF %s, 0\n", ast.expr.args[i].name);
					strcatf(code, "\tMOVWF %s\n", ast.func.args[i].name);

				// Pass const
				} else {
					if(ast.expr.args[i].type == INT_VAR){
						strcatf(code, "\tMOVLW %d;\n", ast.expr.args[i].int_value);
					} else if(ast.expr.args[i].type == CHAR_VAR){
						strcatf(code, "\tMOVLW '%c';\n", ast.expr.args[i].char_value);
					} else if(ast.expr.args[i].type == STR_VAR){
					} else {
						strcatf(code, "\tMOVF %d, 0;\n", asm_var_addr(ast.func.args[i].name));
						strcatf(code, "\tMOVWF %s;\n", ast.func.args[i].name);
					}

					strcatf(code, "\tMOVWF %s\n", ast.func.args[i].name);
				}

			}

			strcatf(code, "\tCALL %s", ast.expr.caller.name);
			// *length = 1;
			break;

		case AST_STATEMENT:
			if(ast.expr.mono_side){
				if(ast.expr.left.arithmetic == 1){
					sprintf(code, "\tINCF %s, 1", ast.expr.left.var.name);
				} else if (ast.expr.left.arithmetic == -1){
					sprintf(code, "\tDECF %s, 1", ast.expr.left.var.name);
				}

			} else {
				if(ast.expr.type == EXPR_ASSIGNABLE){
					strcatf(code, "\tMOVLW 0x%x\n", ast.expr.left.value);
					strcatf(code, "\tMOVWF %s\n", ast.expr.assign_name);
				}
			}
			break;


		case AST_RAW_ASM:
			sprintf(code, "\t%s  ; Raw ASM", ast.raw_asm);
			break;

		case AST_NO_STATEMENT: break;
	}
}

/*
void remove_range(int start, int end) {
	int range_size = end - start + 1;
	int total_rows = ast_len();
	for (int i = end + 1; i < total_rows; ++i) {
		strcpy(tree[i - range_size], tree[i]);
	}
	total_rows -= range_size;
	for (int i = total_rows; i < total_rows + range_size; ++i) {
		tree[i][0] = '\0';
	}
	set_ast_len(total_rows);
}


void remove_unused_func(char name[]){
	int i = 0;
	char tmp[128] = { 0 };
	int start = -1, end = -1;

	// strcatf(tmp, "%s:", name);
	sprintf(tmp, "%s:", name);

	for(i = 0; i < ast_len(); ++i){
		if(strcmp(tree[i], tmp) == 0){ start = i; }
		if(strstr(tree[i], "RETLW") && start != -1){ end = i; }
	}
	if(start != -1 && end != -1){
		remove_range(start, end);
	}
}
*/
