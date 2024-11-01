#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "rules.h"
#include "utility.h"

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

void code_emission(AST ast, char code[]){

	char tmp[MAXSIZ] = { 0 };

	int current = 0;

	switch(ast.type){
		case AST_VARIABLE_ASSIGNMENT:
			if(ast.asgmt.is_func == 0 && ast.asgmt.is_str == 0){
				if(ast.asgmt.type == INT_VAR || ast.asgmt.type == CHAR_VAR){
					int addr = pop_ram();
					// sprintf(code, "%s EQU 0x%x", ast.asgmt.name, addr);
					strcatf(code, "%s EQU 0x%x\nMOVLW 0x%x\nMOVWF %s", ast.asgmt.name, addr, ast.asgmt.value, ast.asgmt.name);
					insts += 3;
					// if(ast.asgmt.value != 0){
					// }
				}
			}
			
			break;

		case AST_FOR_LOOP_ASSIGNMENT:
			break;

		case AST_WHILE_LOOP_ASSIGNMENT:
			break;

		case AST_IF_STATEMENT:
			current = insts;
			if(ast.cond.op == 0){  // ==
				strcatf(code, "\tMOVF %s, F\n", ast.cond.left); insts++;
				strcatf(code, "\tSUBWF %s, W\n", ast.cond.right); insts++;
				strcatf(code, "\tBTFSS STATUS, Z\n"); insts++;
			printf("CURRENT: %d\n", insts);
				strcatf(code, "\t\tNOP\n");
				strcatf(code, "\t\tNOP\n");
			}
			break;

		case AST_ELSE_STATEMENT:
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
			break;

		case AST_FUNCTION_CALL:
			sprintf(code, "\tCALL %s", ast.expr.caller.name);
			break;

		case AST_STATEMENT:
			if(ast.expr.mono_side){
				if(ast.expr.left.arithmetic == 1){
					sprintf(code, "\tINCF %s, W", ast.expr.left.var.name);
				} else if (ast.expr.left.arithmetic == -1){
					sprintf(code, "\tDECF %s, W", ast.expr.left.var.name);
				}
			}
			break;


		case AST_RAW_ASM:
			sprintf(code, "%s", ast.raw_asm);
			break;

		case AST_NO_STATEMENT: break;
	}
}

