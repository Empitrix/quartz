#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "rules.h"

// RAM address
static int ram_stack[MAX_RAM + 1] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x00};


/* pop_ram: pop one ram address (if address is 0 it means that this is an invalid address)*/
int pop_ram(){
	int out = ram_stack[0];
	for(int i = 0; i < MAX_RAM - 1; ++i){
		ram_stack[i] = ram_stack[i + 1];
	}
	return out;
}

void code_emission(AST ast, char code[]){
	switch(ast.type){
	case AST_VARIABLE_ASSIGNEMNT:
		if(ast.asgmt.is_func == 0 && ast.asgmt.is_str == 0){
			if(ast.asgmt.type == INT_VAR){
				int addr = pop_ram();
				sprintf(code, "%s EQU 0x%x", ast.asgmt.name, addr);
				if(addr == 0){
					printf("Too many variable definition (out of ram)\n"); exit(0);
				}
			}
		}
		break;

	case AST_FOOR_LOOP_ASSIGNEMNT:
		break;

	case AST_WHILE_LOOP_ASSIGNEMNT:
		break;

	case AST_IF_STATEMENT:
		break;

	case AST_ELSE_STATEMENT:
		break;

	case AST_FUNCTION_ASSIGNEMNT:
		sprintf(code, "%s:", ast.func.name);
		break;

	case AST_RETURN_STATEMENT:
		sprintf(code, "\tRETLW %d", ast.value.int_value);
		break;

	case AST_FUNCTION_CALL:
		break;

	case AST_STATEMENT:
		if(ast.expr.mono_side){
			if(ast.expr.left.arithmetic == 1){
				sprintf(code, "\tINCF %d, W", ast.expr.left.arithmetic);
			} else if (ast.expr.left.arithmetic == -1){
				sprintf(code, "\tDECF %d, W", ast.expr.left.arithmetic);
			}
		}
		break;

	case AST_NO_STATEMENT:
		break;
	}
}

