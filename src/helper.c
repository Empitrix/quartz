#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



void pass_by_type(TKNS *tkns, token_t type, const char *msg, const char *exp){
	if(tkns->tokens[tkns->idx].type == type){
		tkns->idx++;
	} else {
		throw_err(tkns, msg, exp);
		exit(0);
	}
}


variable_t get_type(char type[]){
	if(strcmp(type, "int") == 0){
		return INT_TYPE;
	} else if (strcmp(type, "char") == 0){
		return CHAR_TYPE;
	} else {
		return INVALID_TYPE;
	}
}


void skip_white_space(TKNS *tkns){
	if(tkns->tokens[tkns->idx].type == WHITESPACE){ tkns->idx++; }
}

void skip_gap(TKNS *tkns){
	while (tkns->tokens[tkns->idx].type == WHITESPACE || tkns->tokens[tkns->idx].type == NEWLINE) {
		tkns->idx++;
	}
}


void get_brace_content(TKNS *tkns, TKNS *save){
	int open = 0;
	save->idx = 0;
	save->max = 0;
	int sidx = tkns->idx;

	while(tkns->tokens[tkns->idx].type != BRACE_CLS){

		if(tkns->tokens[tkns->idx].type == BRACE_OPN){ open++; }
		if(tkns->tokens[tkns->idx].type == BRACE_CLS){ open--; }


		tkns->idx++;
		if(tkns->tokens[tkns->idx].type == BRACE_CLS && open == 0){ break; }

		
		if(tkns->idx >= tkns->max){
			if(open != 0){
				tkns->idx = sidx;
				throw_err(tkns, "syntax error", "'}' or closed brace at end of the file");
				exit(0);
			} else {
				return;
			}
		} else {
			save->tokens[save->max] = tkns->tokens[tkns->idx];
			save->max++;
		}
	}
}




CNST_VAR const_var(TKNS *tkns){
	CNST_VAR cvar;
	cvar.type = INT_CONST;

	if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		if(get_literal_value(tkns->tokens[tkns->idx].word, &cvar.int_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid value", "literal value"); exit(0);
		}
	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		if(get_char_value(tkns, &cvar.char_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid character", NULL); exit(0);
		}

	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		if(get_string(tkns, cvar.str_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid string constant", NULL); exit(0);
		}
	} else {
		throw_err(tkns, "Invalid constant", NULL); exit(0);
	}
	return cvar;
}


/* start's from [... => ] = "..."; */
void str_var_asgmt(TKNS *tkns, char *value){
	// string
	tkns->idx++;
	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == BRAKET_CLS){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid syntax", "]");
		exit(0);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid syntax", "=");
		exit(0);
	}


	skip_white_space(tkns);

	if(get_string(tkns, value) == 0){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid syntax", NULL);
		exit(0);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == END_SIGN){
		++tkns->idx;
		return;
	} else {
		throw_err(tkns, "Invalid syntax", ";");
		exit(0);
	}
}



/* start's from =... => = <int>; */
void u8_var_asgmt(TKNS *tkns, int *value, variable_t type){

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid symbol", "="); exit(0);
	}

	skip_white_space(tkns);

	if(type == INT_TYPE){
		if(tkns->tokens[tkns->idx].type == INTEGER_VALUE && get_literal_value(tkns->tokens[tkns->idx].word, value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "invalid value", "literal value"); exit(0);
		}
	} else {
		get_char_value(tkns, (char *)value);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == END_SIGN){
		tkns->idx++;
		return;
	} else {
		throw_err(tkns, "syntax error", ";");
		exit(0);
	}
}


/* var_asgmt: return structure for 'int', 'char', 'char[]' or function assignment */
ASGMT var_asgmt(TKNS *tkns){
	ASGMT ae;
	ae.is_str = 0;
	ae.is_func = 0;
	ae.type = INVALID_TYPE;

	if((ae.type = get_type(tkns->tokens[tkns->idx].word)) != INVALID_TYPE){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid type", "int, char, char[]"); exit(0);
	}
 
	skip_white_space(tkns);

	// get varialbe name
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		strcpy(ae.name, tkns->tokens[tkns->idx].word);
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid varialbe name", NULL); exit(0);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
		// String detected
		str_var_asgmt(tkns, ae.str);
		ae.is_str = 1;
		printf("VALUE: >%s<\n", ae.str);
		return ae;

	} else if(tkns->tokens[tkns->idx].type == PAREN_OPN){
		// Function detected

	} else if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		// Variable detected
		u8_var_asgmt(tkns, &ae.value, ae.type);
		printf("VALUE: >%d<\n", ae.value);
		return ae;

	} else {
		printf("%s - %d\n", tkns->tokens[tkns->idx].word, tkns->tokens[tkns->idx].type);
		// Invalid
		throw_err(tkns, "Invalid symbol", NULL);
		exit(0);
	}

	return ae;
}



MACRO macro_asgmt(TKNS *tkns){
	MACRO mcro;
	mcro.value.type = INT_CONST;

	tkns->idx++;

	if(tkns->tokens[tkns->idx].type == INCLUDE_KEWORD || tkns->tokens[tkns->idx].type == DEFINE_KEWORD){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid preprocessor directive", "include or define");
		exit(0);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		strcpy(mcro.name, tkns->tokens[tkns->idx].word);
		tkns->idx++;
	} else {
		throw_err(tkns, "A name required", NULL);
		exit(0);
	}


	skip_white_space(tkns);

	mcro.value = const_var(tkns);

	skip_white_space(tkns);


	if(tkns->tokens[tkns->idx].type == NEWLINE){
		printf("MACRO: %s\n", mcro.name);
		return mcro;
	} else {
		throw_err(tkns, "Invalid macro", NULL);
		exit(0);
	}

	return mcro;
}


operator set_double_op(TKNS *tkns, token_t tok, operator s, operator d){
	if(tkns->tokens[tkns->idx + 1].type == tok){
		return d;
	} else {
		return s;
	}
}

operator get_operator(TKNS *tkns){
	operator op = NO_OP;
	switch(tkns->tokens[tkns->idx].type){
		case PLUS_SIGN:
			op = set_double_op(tkns, PLUS_SIGN, ADD_OP, INCREMENT_OP);
			break;

		case MINUS_SIGN:
			op = set_double_op(tkns, MINUS_SIGN, MINUS_OP, DECREMENT_OP);
			break;

		case EQUAL_SIGN:
			op = set_double_op(tkns, EQUAL_SIGN, ASSIGN_OP, EQUAL_OP);
			break;

		case LEFT_SIGN:
			if(tkns->tokens[tkns->idx + 1].type == EQUAL_SIGN){
				op = SMALLER_EQ_OP;
			} else {
				op = set_double_op(tkns, LEFT_SIGN, SMALLER_OP, SHIFT_LEFT_OP);
			}
			break;


		case RIGHT_SIGN:
			if(tkns->tokens[tkns->idx + 1].type == EQUAL_SIGN){
				op = GREATOR_EQ_OP;
			} else {
				op = set_double_op(tkns, RIGHT_SIGN, GREATOR_EQ_OP, SHIFT_RIGHT_OP);
			}
			break;

		case TILDE_SIGN:
			op = COMPLEMENT_OP;
			break;
		default:
			op = INVALID_OP;
	}
	// if(op != INVALID_OP){ tkns->idx ++; }
	return op;
}


void skip_double_op(TKNS *tkns, operator op){
	if(op == INCREMENT_OP ||
			op == DECREMENT_OP ||
			op == EQUAL_OP ||
			op == SMALLER_EQ_OP ||
			op == GREATOR_EQ_OP ||
			op == SHIFT_RIGHT_OP ||
			op == SHIFT_LEFT_OP){
		tkns->idx++;
	}
}

STMT get_statement(TKNS *tkns){
	STMT st;

	skip_white_space(tkns);


	if(tkns->tokens[tkns->idx].type == END_SIGN){
		st.op = NO_OP;
		return st;
	}

	st.op = get_operator(tkns);

	// TODO: variable name
	pass_by_type(tkns, IDENTIFIER, "Invalid syntax", NULL);
	strcpy(st.left, tkns->tokens[tkns->idx - 1].word);

	skip_white_space(tkns);

	if(st.op == COMPLEMENT_OP){ return st; }

	st.op = get_operator(tkns);
	if(st.op == INVALID_OP || st.op == COMPLEMENT_OP){
		throw_err(tkns, "Invalid operator", NULL);
		exit(0);
	} else {

		st.op = get_operator(tkns);
		// tkns->idx++;
		// skip_double_op(tkns, st.op);

		if(st.op == INVALID_OP || st.op == NO_OP){
			throw_err(tkns, "Invalid operator", NULL);
			exit(0);
		} else if(st.op == INCREMENT_OP || st.op == DECREMENT_OP){
			tkns->idx += 2;
			return st;
		} else {
			tkns->idx++;
		}

		skip_double_op(tkns, st.op);
		skip_white_space(tkns);
		pass_by_type(tkns, INTEGER_VALUE, "Invalid syntax", "integer");
		strcpy(st.right, tkns->tokens[tkns->idx - 1].word);
	}

	return st;
}



FOR_ASGMT for_asgmt(TKNS *tkns){
	FOR_ASGMT fr;
	tkns->idx++;
	skip_white_space(tkns);

	// initial
	pass_by_type(tkns, PAREN_OPN, "Invalid character", "'('");
	skip_white_space(tkns);
	fr.init = get_statement(tkns);

	skip_white_space(tkns);

	// condition
	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
	skip_white_space(tkns);
	fr.cond = get_statement(tkns);

	skip_white_space(tkns);

	// iterator
	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
	skip_white_space(tkns);
	fr.iter = get_statement(tkns);

	skip_white_space(tkns);


	// end )
	pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");

	skip_white_space(tkns);

	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	// skip_gap(tkns)

	get_brace_content(tkns, &fr.body);  // get everything in for loop as tokens

	// for(int i = 0; i < fr.tkns.max; i++){
	// 	printf("%s\n", fr.tkns.tokens[i].word);
	// }


	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");
	
	return fr;
}


BODY_ASGMT body_asgmt(TKNS *tkns, body_t type){
	BODY_ASGMT wa;
	wa.type = type;
	tkns->idx++;
	skip_white_space(tkns);

	// (...), condition (test) part of the while loop
	pass_by_type(tkns, PAREN_OPN, "Invalid character", "'('");
	skip_white_space(tkns);
	wa.cond = get_statement(tkns);
	skip_white_space(tkns);
	pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");

	skip_white_space(tkns);

	// body of the loop
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	get_brace_content(tkns, &wa.body);
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	return wa;
}


BODY_ASGMT else_asgmt(TKNS *tkns){
	BODY_ASGMT wa;
	wa.type = ELSE_BODY;
	tkns->idx++;

	skip_white_space(tkns);

	// body of the loop
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	get_brace_content(tkns, &wa.body);
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	return wa;
}


int return_asgmt(TKNS *tkns){
	tkns->idx++;

	int value = 0;
	skip_white_space(tkns);

	if(get_literal_value(tkns->tokens[tkns->idx].word, &value) == 0){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid value", "integer");
		exit(0);
	}

	skip_white_space(tkns);
	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
	return value;
}
