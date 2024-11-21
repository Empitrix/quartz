#include "rules.h"
#include "types.h"
#include "utility.h"
#include "helper.h"


void generate_for(Qast *asts, int *i, ast_t type, int move_back);
void assign_func_arg(Qast *ast);
void set_condition(SNIP *, int);



void generator(Qast *asts, int start, int end){
	int i;
	int main_detect = 0;

	for(i = start; i < end; ++i){

		// Variable and macro
		if(asts[i].type == AST_VARIABLE_ASSIGNMENT){
			attf("%s EQU 0x%.2X", asts[i].snip.assigned.name, asts[i].snip.assigned.addr);
			attf("MOVLW 0x%.2X", asts[i].snip.assigned.numeric_value);
			attf("MOVWF %s", asts[i].snip.assigned.name);


		} else if(asts[i].type == AST_FUNCTION_ASSIGNMENT){
			attf("%s:", asts[i].snip.assigned.name);
			main_detect = strcmp(asts[i].snip.assigned.name, "main") == 0;
			generate_for(asts, &i, asts[i].type, 1);  // move back by 1 becase we need to handle "return"


		} else if(asts[i].type == AST_IF_STATEMENT){
			set_condition(&asts[i].qif.cond, 0);  // set conditions (no reverse)
			const char *if_l = get_label();
			const char *else_l = get_label();
			const char *skip_l = get_label();
			attf("\tGOTO %s", if_l);
			attf("\tGOTO %s", else_l);
			attf("%s:", if_l);

			int save = i;
			if(empty_body(&asts[i].qif.if_body)){
				attf("\tNOP ; empty \"if\" body");
			} else {
				generate_for(asts, &i, asts[i].type, 0);
			}

			attf("\tGOTO %s", skip_l);  // GOTO skip label
			attf("%s:", else_l);

			if(empty_body(&asts[save].qif.else_body)){
				attf("\tNOP ; empty \"else\" body");
			} else {
				++i;  // SKIP "else" keyword
				generate_for(asts, &i, asts[i].type, 0);
			}
			attf("%s:", skip_l);  // set "SKIP" label


		} else if(asts[i].type == AST_FUNCTION_CALL){
			assign_func_arg(&asts[i]);
			attf("\tCALL %s", asts[i].snip.func.name);


		} else if(asts[i].type == AST_RAW_ASM){
			attf("\t%s", asts[i].rasm);


		} else if(asts[i].type == AST_RETURN_STATEMENT){
			if(main_detect){
				attf("\tSLEEP  ; End of the program (main func)");
			} else {
				attf("\tRETLW 0x%.2X", asts[i].var.numeric_value);
			}
		}

	}

}


/* generate_for: looks nodes after the current on that have a type of "type" and generate code for them */
void generate_for(Qast *asts, int *idx, ast_t type, int move_back){
	int i = *idx;
	i++;  // skip itself
	while(asts[i].refer == type){
		++i;
	}

	generator(asts, *idx + 1, i - move_back);
	*idx = i - (move_back + 1);
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


/* loads the literal (const) to W and return the var that has an address */
/* if both of the have a valid address loads one into W and returns the other */
Qvar *load_to_w(Qvar *a, Qvar *b){
	Qvar *ret = a;
	int a_lit = 0, b_lit = 0;

	if(const_type(a->type)){ a_lit = 1; }
	if(const_type(b->type)){ b_lit = 1; }

	if(a_lit){
		attf("\tMOVLW 0x%.2X", a->numeric_value);
		ret = b;

	} else if(b_lit){
		attf("\tMOVLW 0x%.2X", b->numeric_value);
		ret = a;

	} else {
		attf("\tMOVF 0x%.2X, 0", a->addr);
		ret = b;
	}

	return ret;
};

void set_cond_test(operator op, int reverse){
	if(op == EQUAL_OP){
		if(reverse){
			attf("\tBTFSC STATUS, Z");
		} else {
			attf("\tBTFSS STATUS, Z");
		}
	}
}

void set_condition(SNIP *snip, int reverse){
	if(snip->type != CONDITIONAL_SNIP){ return ; }
	
	Qvar *q = load_to_w(&snip->left, &snip->right);

	if(snip->op == EQUAL_OP){
		attf("\tXORWF %s, W", q->name);
		set_cond_test(snip->op, reverse);
	}


}

