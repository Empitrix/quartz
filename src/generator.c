#include "rules.h"
#include "types.h"
#include "utility.h"
#include "helper.h"
#include "emission.h"


// void generate_for(Qast *asts, int *i, ast_t type, int move_back);
void generate_for(Qast *asts, int *i);
void assign_func_arg(Qast *ast);
void set_condition(SNIP *, int);

void load_var(Qvar *);


static int main_detect = 0;

void generator(Qast *asts, int start, int end){
	int i;

	for(i = start; i < end; ++i){

		// Variable and macro
		if(asts[i].type == AST_VARIABLE_ASSIGNMENT){
			attf("%s EQU 0x%.2X", asts[i].snip.assigned.name, asts[i].snip.assigned.addr);
			attf("MOVLW 0x%.2X", asts[i].snip.assigned.numeric_value);
			attf("MOVWF %s", asts[i].snip.assigned.name);


		} else if(asts[i].type == AST_FUNCTION_ASSIGNMENT){
			attf("%s:", asts[i].snip.assigned.name);
			main_detect = strcmp(asts[i].snip.assigned.name, "main") == 0;
			generate_for(asts, &i);


		} else if(asts[i].type == AST_IF_STATEMENT){
			set_condition(&asts[i].qif.cond, 0);  // set conditions (no reverse)
			const char *if_l = get_label();
			const char *else_l = get_label();
			const char *skip_l = get_label();
			attf("\tGOTO %s", if_l);             // Goto EQUAL
			attf("\tGOTO %s", else_l);           // Goto NOT EQUAL
			attf("%s:", if_l);                   // Set "if" label
			int save = i;
			if(empty_body(&asts[i].qif.if_body)){
				attf("\tNOP ; empty \"if\" body");
			} else {
				generate_for(asts, &i);
			}

			attf("\tGOTO %s", skip_l);           // GOTO skip labe (end of equal)
			attf("%s:", else_l);                 // Set "else" label

			if(empty_body(&asts[save].qif.else_body)){
				attf("\tNOP ; empty \"else\" body");
			} else {
				++i;  // SKIP "else" keyword
				generate_for(asts, &i);
			}
			attf("%s:", skip_l);  // set "SKIP" label


		} else if(asts[i].type == AST_FOR_LOOP_ASSIGNMENT){
			const char *top_loop = get_label();
			const char *skip_loop = get_label();

			gen_assign(&asts[i].qfor.init);           // generate init part
			attf("%s:", top_loop);                    // Set top of the loop label (no init part)
			set_condition(&asts[i].qfor.cond, 0);     // set conditions
			attf("\tGOTO %s", skip_loop);             // skip the loop (if not valid)
			int save = i;
			generate_for(asts, &i);                   // Generate for body
			gen_iter(&asts[save].qfor.iter);          // Generate iter part
			attf("\tGOTO %s", top_loop);              // Goto top of the loop
			attf("%s:", skip_loop);                   // label to skip the loop


		} else if(asts[i].type == AST_WHILE_LOOP_ASSIGNMENT){
			const char *top_loop = get_label();
			const char *skip_loop = get_label();
			attf("%s:", top_loop);                      // Set a label for top of the loop
			set_condition(&asts[i].qwhile.cond, 0);     // Set conditions
			attf("\tGOTO %s", skip_loop);               // Exit the loop (test failed)
			generate_for(asts, &i);                     // Generate for "while" body
			attf("\tGOTO %s", top_loop);                // Goto top of the loop
			attf("%s:", skip_loop);                     // Set a label to skip the loop


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


		} else if(asts[i].type == AST_STATEMENT){
			if(asts[i].snip.type == ITTERATIONAL_SNIP){
				gen_iter(&asts[i].snip);

			} else {
				if(asts[i].snip.assigne_type != UPDATE_ASG){ continue; }

				// =
				if(asts[i].snip.op == ASSIGN_OP){
					if(const_type(asts[i].snip.assigned.type) == 0){
						load_var(&asts[i].snip.left);
						attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);
					}
				}
			}
		}


	}

}


/* generate_for: looks nodes after the current one that have a type of "type" and generate code for them */
void generate_for(Qast *asts, int *idx){
	int save = *idx;
	generator(asts, save + 1, *idx + asts[save].children + 1);
	*idx = *idx + asts[save].children;
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


void load_var(Qvar *v){
	if(const_type(v->type)){
		attf("\tMOVLW 0x%.2X", v->numeric_value);
	} else {
		attf("\tMOVF 0x%.2X, W", v->addr);
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
		// attf("\tMOVF 0x%.2X, 0", a->addr);
		attf("\tMOVF %s, W", b->name);
		ret = a;
	}

	return ret;
}


const char *get_test(int reverse){
	if(reverse){
		return "BTFSS";
	}
	return "BTFSC";
}


void set_condition(SNIP *snip, int reverse){
	if(snip->type != CONDITIONAL_SNIP){ return ; }
	
	Qvar *q = load_to_w(&snip->left, &snip->right);

	if(snip->op == EQUAL_OP){
		attf("\tXORWF %s, W", q->name);
		attf("\t%s STATUS, Z", get_test(reverse == 0));

	} else if(snip->op == SMALLER_EQ_OP){
		attf("\tSUBWF %s, W", q->name);
		attf("\t%s STATUS, C", get_test(reverse));

	}
}

