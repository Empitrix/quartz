#include "types.h"
#include "utility.h"
#include "helper.h"


void extract_idx(Qast *asts, int idx, int max, int *start, int *end);
void assign_func_arg(Qast *ast);



void generator(Qast *asts, int start, int end){
	int i;

	int idx_start = 0, idx_end = 0;
	int main_detect = 0;

	for(i = start; i < end; ++i){

		idx_start = 0; idx_end = 0;

		// Variable and macro
		if(asts[i].type == AST_VARIABLE_ASSIGNMENT){
			attf("%s EQU 0x%.2X", asts[i].snip.assigned.name, asts[i].snip.assigned.addr);

		} else if(asts[i].type == AST_FUNCTION_ASSIGNMENT){
			attf("%s:", asts[i].snip.assigned.name);
			extract_idx(asts, i, end, &idx_start, &idx_end);
			generator(asts, idx_start, idx_end);
			i = idx_end;
			main_detect = strcmp(asts[i].snip.assigned.name, "main") == 0;

		} else if(asts[i].type == AST_FUNCTION_CALL){
			assign_func_arg(&asts[i]);
			attf("\tCALL %s", asts[i].snip.func.name);

		} else if(asts[i].type == AST_RAW_ASM){
			attf("\t%s", asts[i].rasm);

		} else if(asts[i].type == AST_RETURN_STATEMENT){
			if(main_detect){
				attf("\tSLEEP");
			} else {
				attf("\tRETLW 0x%.2X", asts[i].var.numeric_value);
			}

		}

	}
}


/* set "end" and "start" for the children of given AST */
void extract_idx(Qast *asts, int idx, int max, int *start, int *end){
	int i;
	*start = idx;
	for(i = idx; i < max; i++){
		if(asts[i].refer != asts[idx].type){
			*end  = i;
			break;
		}
	}
}


/* set or load values / variable to calling function address */
void assign_func_arg(Qast *ast){
	for(int j = 0; j < ast->snip.func.arg_len; ++j){
		assign_var(ast->snip.func.args[j].name, ast->snip.func.args[j].addr);
		if(const_type(ast->snip.args[j].type)){
			attf("\tMOVLW 0x%.2X", ast->snip.args[j].numeric_value);
		} else {
			attf("\tMOVF %s, W", ast->snip.args[j].name);
		}
		attf("\tMOVWF %s", ast->snip.func.args[j].name);
	}
}
