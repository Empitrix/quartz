#include "rules.h"
#include "types.h"
#include "helper.h"
#include <stdio.h>


void gen_assign(SNIP *snip){
	if((snip->type != ASSIGNMENT_SNIP || snip->type != NOT_EFFECTIVE_SNIP) &&
		snip->assigne_type == NO_ASSIGNMENT_ASG){ return; }

	if(snip->assigne_type == UPDATE_ASG){
		
		attf("\tMOVLW 0x%.2X", snip->left.numeric_value);
		attf("\tMOVWF %s", snip->assigned.name);
	}

}

void gen_iter(SNIP *snip){
	if(snip->type != ITTERATIONAL_SNIP){ return; }

	if(snip->op == INCREMENT_OP){
		attf("\tINCF %s, F", snip->left.name);
	} else {
		attf("\tDECF %s, F", snip->left.name);
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

void set_com(Qvar *v){
	if(v->com && const_type(v->type) == 0){
		attf("\tCOMF %s, F", v->name);
	}
}

void load_var(Qvar *v){
	if(const_type(v->type)){
		attf("\tMOVLW 0x%.2X", v->numeric_value);
	} else {
		set_com(v);
		attf("\tMOVF 0x%.2X, W", v->addr);
	}
}


/* Load a value (can be from Q-var or Q-const) to compiler reserved RAM */
void load_cram(Qvar *v){
	if(const_type(v->type)){
		attf("\tMOVLW 0x%.2X", v->numeric_value);
		attf("\tMOVWF 0x%.2X", CRAM);
	} else {
		attf("\tMOVF %s, W", v->name);
		attf("\tMOVWF 0x%.2X", CRAM);
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
		return "BTFSC";
	}
	return "BTFSS";
}


void set_condition(SNIP *snip, int reverse){
	if(snip->type != CONDITIONAL_SNIP){ return ; }

	// ==
	if(snip->op == EQUAL_OP){
		Qvar *q = load_to_w(&snip->left, &snip->right);
		attf("\tXORWF %s, W", q->name);
		attf("\t%s STATUS, Z", get_test(reverse == 0));

	// !=
	} else if(snip->op == NOT_EQUAL_OP){
		Qvar *q = load_to_w(&snip->left, &snip->right);
		attf("\tXORWF %s, W", q->name);
		attf("\t%s STATUS, Z", get_test(reverse == 0));

	// <=
	} else if(snip->op == SMALLER_EQ_OP){
		load_cram(&snip->right);
		load_var(&snip->left);
		attf("\tSUBWF 0x%.2X, W", CRAM);
		attf("\t%s STATUS, C", get_test(reverse == 1));

	// <
	} else if(snip->op == SMALLER_OP){
		load_cram(&snip->left);
		load_var(&snip->right);
		attf("\tSUBWF %s, W", CRAMS);
		attf("\t%s STATUS, C", get_test(reverse == 0));

	// >=
	} else if(snip->op == GREATOR_EQ_OP){
		load_cram(&snip->left);
		load_var(&snip->right);
		attf("\tSUBWF 0x%.2X, W", CRAM);
		attf("\t%s STATUS, C", get_test(reverse == 1));

	// >
	} else if(snip->op == GREATOR_OP){
		load_cram(&snip->right);
		load_var(&snip->left);
		attf("\tSUBWF %s, W", CRAMS);
		attf("\t%s STATUS, C", get_test(reverse == 0));

	}
}


// operate shift '<<' and '>>'
void set_shift(SNIP *snip, const char *opcode){
	const char *ltop = get_label();
	const char *ldwn = get_label();
	attf("%s:", ltop);


	load_cram(&snip->right);
	int tram = pop_ram();
	attf("\tMOVF 0x%.2X, W", tram);
	attf("\tXORWF %s, W", CRAMS);
	attf("\tBTFSC STATUS, Z");
	attf("\tGOTO %s", ldwn);
	// body
	attf("\t%s %s, W", opcode, snip->left.name);
	attf("\tMOVWF %s", snip->assigned.name);
	attf("\tINCF 0x%.2X, F", tram);
	attf("\tGOTO %s", ltop);

	attf("%s:", ldwn);
	push_ram();  // Free tram
	attf("\tCLRF 0x%.2X", tram);
}
