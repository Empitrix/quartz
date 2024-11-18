#include "types.h"
#include "utility.h"
#include "global.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

EXPR get_expr(TKNS *, token_t);


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



/* pass_by_type: Check if the token has the given type; if it does, move to the next one; otherwise, throw an error. */
void pass_by_type(TKNS *tkns, token_t type, const char *msg, const char *exp){
	if(tkns->tokens[tkns->idx].type == type){
		tkns->idx++;
	} else { throw_err(tkns, msg, exp); }
}


/* skip_whitespace: skip white space */
void skip_whitespace(TKNS *tkns){
	if(tkns->tokens[tkns->idx].type == WHITESPACE){ tkns->idx++; }
}

/* check next two tokens if match return 1 otherwise return 0 (**no effect on TKNS.idx**)*/
int see_forward(TKNS *tkns, int start, token_t a, token_t b){
	start = tkns->idx + start;
	if(tkns->tokens[start].type == a){
		if(tkns->tokens[start + 1].type == b){
			return 1;
		}
	}
	return 0;
}

/* skip_whitespace: skip white space + Newline */
void skip_gap(TKNS *tkns){
	while (tkns->tokens[tkns->idx].type == WHITESPACE || tkns->tokens[tkns->idx].type == NEWLINE) {
		tkns->idx++;
	}
}


var_t get_type(TKNS *tkns){
	var_t v = INT_VAR;
	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == INT_KEYWORD){
		tkns->idx++;
		v = INT_VAR;
		// int
		return v;
	}

	if(tkns->tokens[tkns->idx].type == CHAR_KEYWORD){
		tkns->idx++;
		v = CHAR_VAR;
		skip_whitespace(tkns);

		if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
			tkns->idx++;
			skip_whitespace(tkns);
			pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
			v = STR_VAR;
			// str
			return v;
		} else {
			// char
			return v;
		}
		
	} else { throw_err(tkns, "Invalid type", "int, char, char[]"); }

	return v;
}


int contains_str_type(TKNS *tkns, var_t *type){
	if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
		tkns->idx++;
		skip_whitespace(tkns);
		pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
		skip_whitespace(tkns);
		*type = STR_VAR;
		return 1;
	}
	return 0;
}


// /* cet_var_arg: get function arguments */
// int get_var_arg(TKNS *tkns, char name[], var_t *type){
// 
// 	skip_whitespace(tkns);
// 
// 	if(tkns->tokens[tkns->idx].type == COMMA_SIGN){ return 0; }
// 
// 
// 	if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0 ||
// 		strcmp(tkns->tokens[tkns->idx].word, "char") == 0){
// 
// 		tkns->idx++;
// 		
// 		if(strcmp(tkns->tokens[tkns->idx].word, "int") == 0){
// 			*type = INT_VAR;
// 		} else {
// 			*type = CHAR_VAR;
// 		}
// 
// 
// 		skip_whitespace(tkns);
// 
// 		if(tkns->tokens[tkns->idx].type == IDENTIFIER){
// 			strcpy(name, tkns->tokens[tkns->idx].word);  // update name
// 			update_name_state(tkns, name, 1);
// 			tkns->idx++;
// 		} else {
// 			throw_err(tkns, "Invalid varialbe name", NULL);
// 			exit(0);
// 		}
// 
// 		skip_whitespace(tkns);
// 
// 
// 		if(tkns->tokens[tkns->idx].type == COMMA_SIGN){
// 			return 0;
// 		}
// 
// 		if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
// 			tkns->idx++;
// 			skip_whitespace(tkns);
// 			pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
// 			skip_whitespace(tkns);
// 			*type = STR_VAR;
// 		}
// 
// 		if(tkns->tokens[tkns->idx].type == COMMA_SIGN){
// 			tkns->idx++;
// 		}
// 
// 		return 0;
// 
// 	} else {
// 		return 1;
// 	}
// 
// 	return 1;
// }


/* set function arguments using 'get_var_arg' */
void get_arg(TKNS *tkns, ARG args[], int *arg_len){
	*arg_len = 0;

	while(tkns->tokens[tkns->idx].type != PAREN_CLS){

		skip_whitespace(tkns);

		args[*arg_len].type = get_type(tkns);
		skip_whitespace(tkns);

		pass_by_type(tkns, IDENTIFIER, "Invalid name", NULL);
		strcpy(args[*arg_len].name, tkns->tokens[tkns->idx - 1].word);
		update_name_state(tkns, args[*arg_len].name, 1);
		skip_whitespace(tkns);

		contains_str_type(tkns, &args[*arg_len].type);

		*arg_len = *arg_len + 1;

		if(tkns->tokens[tkns->idx].type == COMMA_SIGN){
			tkns->idx++;
			continue;
		} else if (tkns->tokens[tkns->idx].type == PAREN_CLS){
			break;
		} else { throw_err(tkns, "Invalid syntax", ", or )"); }

	}

	clear_scoop_names();
}


/* get_brace_content: get everything between two '{' and '}' (braces are not part of the extraction)*/
void get_brace_content(TKNS *tkns, TKNS *save){
	int open;
	open = 0;
	save->idx = 0;
	save->max = 0;
	int sidx = tkns->idx;

	while(tkns->tokens[tkns->idx].type != BRACE_CLS || open != 0){

		if(tkns->tokens[tkns->idx].type == BRACE_OPN){ open++; }
		if(tkns->tokens[tkns->idx].type == BRACE_CLS){ open--; }

		if(tkns->idx >= tkns->max){
			tkns->idx = sidx;
			throw_err(tkns, "syntax error", "'}' or closed brace at end of the file");
		} else {
			save->tokens[save->max] = tkns->tokens[tkns->idx];
			save->max++;
			tkns->idx++;
		}
	}
}


void get_paren_content(TKNS *tkns, TKNS *save){
	int open;
	open = 0;
	save->idx = 0;
	save->max = 0;
	int sidx = tkns->idx;

	while(tkns->tokens[tkns->idx].type != PAREN_CLS || open != 0){

		if(tkns->tokens[tkns->idx].type == PAREN_OPN){ open++; }
		if(tkns->tokens[tkns->idx].type == PAREN_CLS){ open--; }

		if(tkns->idx >= tkns->max){
			tkns->idx = sidx;
			throw_err(tkns, "syntax error", "')' or closed brace at end of the file");
		} else {
			save->tokens[save->max] = tkns->tokens[tkns->idx];
			save->max++;
			tkns->idx++;
		}
	}
}



int check_const_var(TKNS *tkns, CNST_VAR *cvar, int force){
	cvar->int_value = 0;
	cvar->char_value = '\0';

	if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		cvar->type = INT_VAR;
		if(get_literal_value(tkns->tokens[tkns->idx].word, &cvar->int_value) == 0){
			tkns->idx++;
			return 1;
		} else {
			if(force){
				throw_err(tkns, "Invalid value", "literal value");
			}
			return -1;
		}
	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		cvar->type = CHAR_VAR;
		if(get_char_value(tkns, &cvar->char_value)){
			return -1;
			if(force){
				throw_err(tkns, "Invalid character", NULL);
			}
		}
	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		cvar->type = STR_VAR;
		if(get_string(tkns, cvar->str_value) == 0){
			tkns->idx++;
			return 1;
		} else {
			if(force){
				throw_err(tkns, "Invalid string constant", NULL);
			}
			return -1;
		}
	} else {
		if(force){ throw_err(tkns, "Invalid constant", NULL); }
		return 0;
	}
	return 0;
}


/* const_var: get a constant variable (dynamic type) for macro (#define) */
CNST_VAR const_var(TKNS *tkns){
	CNST_VAR cvar;
	cvar.int_value = 0;
	cvar.char_value = '\0';

	if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		cvar.type = INT_VAR;
		if(get_literal_value(tkns->tokens[tkns->idx].word, &cvar.int_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid value", "literal value");
		}
	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		cvar.type = CHAR_VAR;
		if(get_char_value(tkns, &cvar.char_value)){
			throw_err(tkns, "Invalid character", NULL);
		}
	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		cvar.type = STR_VAR;
		if(get_string(tkns, cvar.str_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid string constant", NULL);
		}
	} else {
		throw_err(tkns, "Invalid constant", NULL);
	}
	return cvar;
}


/* start's from [... => ] = "..."; */
void str_var_asgmt(TKNS *tkns, char *value){
	// string
	tkns->idx++;
	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == BRAKET_CLS){
		tkns->idx++;
	} else { throw_err(tkns, "Invalid syntax", "]"); }

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		tkns->idx++;
	} else { throw_err(tkns, "Invalid syntax", "="); }


	skip_whitespace(tkns);

	if(get_string(tkns, value) == 0){
		tkns->idx++;
	} else { throw_err(tkns, "Invalid syntax", NULL); }

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == END_SIGN){
		++tkns->idx;
		return;
	} else { throw_err(tkns, "Invalid syntax", ";"); }
}



/* start's from =... => = <int>; */
void u8_var_asgmt(TKNS *tkns, int *value, var_t type){

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid symbol", "=");
	}

	skip_whitespace(tkns);

	if(type == INT_VAR){
		if(tkns->tokens[tkns->idx].type == INTEGER_VALUE && get_literal_value(tkns->tokens[tkns->idx].word, value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "invalid value", "literal value");
		}
	} else {
		// printf("SHOULD OF BEEN CHAR\n");
		get_char_value(tkns, (char *)value);
	}

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == END_SIGN){
		tkns->idx++;
		return;
	} else { throw_err(tkns, "syntax error", ";"); }
}


/* var_asgmt: return structure for 'int', 'char', 'char[]' or function assignment */
ASGMT var_asgmt(TKNS *tkns){
	ASGMT ae;
	ae.is_str = 0;
	ae.address = 0;
	ae.is_func = 0;
	ae.value = 0;
	ae.type = INT_VAR;
	ae.func.arg_len = 0;
	ae.func.return_type = INT_VAR;

	skip_whitespace(tkns);
	ae.type = get_type(tkns);
	skip_whitespace(tkns);

	// get varialbe name
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		strcpy(ae.name, tkns->tokens[tkns->idx].word);
		update_name_state(tkns, ae.name, 0);
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid varialbe name", NULL);
	}

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == BRAKET_OPN){  // String detected
		ae.type = STR_VAR;
		str_var_asgmt(tkns, ae.str);
		ae.is_str = 1;
		return ae;

	} else if(tkns->tokens[tkns->idx].type == PAREN_OPN){ // Function detected
		ae.is_func = 1;
		tkns->idx++;
		get_arg(tkns, ae.func.args, &ae.func.arg_len);
		for(int i = 0; i < ae.func.arg_len; ++i){
			VAR v;
			strcpy(v.name, ae.func.args[i].name);
			v.type = ae.func.args[i].type;
			v.value = 255;
			save_scoop_variable(v);
		}
		pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");
		skip_whitespace(tkns);
		pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
		get_brace_content(tkns, &ae.func.body);  // update function's body
		skip_gap(tkns);
		pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");
		ae.func.return_type = ae.type;
		strcpy(ae.func.name, ae.name);  // set function name
		return ae;

	} else if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){   // variable
		u8_var_asgmt(tkns, &ae.value, ae.type);
		return ae;

	} else { throw_err(tkns, "Invalid symbol", NULL); }

	return ae;
}


/* macro_asgmt: assignment for #include and #define */
MACRO macro_asgmt(TKNS *tkns){
	MACRO mcro;
	mcro.value.type = INT_VAR;
	mcro.type = DEFINE_MACRO;

	tkns->idx++;


	// if(tkns->tokens[tkns->idx].type == INCLUDE_KEYWORD || tkns->tokens[tkns->idx].type == DEFINE_KEYWORD){
	if(tkns->tokens[tkns->idx].type == DEFINE_KEYWORD){
		tkns->idx++;
	// } else { throw_err(tkns, "Invalid preprocessor directive", "include or define"); }
	} else { throw_err(tkns, "Invalid preprocessor directive", "define"); }

	// int is_include = tkns->tokens[tkns->idx - 1].type == INCLUDE_KEYWORD;

	skip_whitespace(tkns);

	/*
	if(is_include){
		mcro.type = INCLUDE_MACRO;


		if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
			if(get_string(tkns, mcro.name) == 0){
				tkns->idx++;
			} else { throw_err(tkns, "Invalid string constant", NULL); }
		} else { throw_err(tkns, "Invalid constant", NULL); }

	} else {

		if(tkns->tokens[tkns->idx].type == IDENTIFIER){
			strcpy(mcro.name, tkns->tokens[tkns->idx].word);

			update_name_state(tkns, mcro.name, 0);

			tkns->idx++;
		} else { throw_err(tkns, "A name required", NULL); }

		skip_whitespace(tkns);
		mcro.value = const_var(tkns);
	}
	*/

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		strcpy(mcro.name, tkns->tokens[tkns->idx].word);

		update_name_state(tkns, mcro.name, 0);

		tkns->idx++;
	} else { throw_err(tkns, "A name required", NULL); }

	skip_whitespace(tkns);
	// mcro.value = const_var(tkns);


	CNST_VAR cvar;
	cvar.int_value = 0;
	cvar.char_value = '\0';

	if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		cvar.type = INT_VAR;
		if(get_literal_value(tkns->tokens[tkns->idx].word, &cvar.int_value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid value", "literal value");
		}
	} else {
		throw_err(tkns, "Invalid \"#define\" value", "integer");
	}

	mcro.value = cvar;

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == NEWLINE){
		return mcro;
	} else { throw_err(tkns, "Invalid macro", NULL); }

	return mcro;
}


/* get_operator: get operator: a == 1 as EQUAL or a = 1 as ASSIGN and so on... */
operator set_double_op(TKNS *tkns, token_t tok, operator s, operator d){
	if(tkns->tokens[tkns->idx + 1].type == tok){
		return d;
	} else {
		return s;
	}
}

/* get_operator: get operator: a == 1 as EQUAL or a = 1 as ASSIGN and so on... */
operator get_operator(TKNS *tkns){
	operator op = NO_OP;
	switch(tkns->tokens[tkns->idx].type){

		case PLUS_SIGN:
			if(tkns->tokens[tkns->idx + 1].type == EQUAL_SIGN){
				return ADD_ASSIGN_OP;
			} else if (tkns->tokens[tkns->idx + 1].type == PLUS_SIGN){
				return INCREMENT_OP;
			}
			return ADD_OP;

		// !=
		case EXCLAMATION_SIGN:
			if(tkns->tokens[tkns->idx + 1].type == EQUAL_SIGN){
				return NOT_EQUAL_OP;
			}
			return NO_OP;


		case MINUS_SIGN:
			// op = set_double_op(tkns, MINUS_SIGN, MINUS_OP, DECREMENT_OP);
			//
			if(tkns->tokens[tkns->idx + 1].type == EQUAL_SIGN){
				return MINUS_ASSIGN_OP;
			}
			return MINUS_OP;
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


		case AND_SIGN:
			return AND_OP;

		case OR_SIGN:
			return OR_OP;

		default:
			op = INVALID_OP;
	}
	return op;
}

/* skip_double_op: skip double operators like ++, --, >=, ...*/
void skip_double_op(TKNS *tkns, operator op){
	if(op == INCREMENT_OP ||
			op == DECREMENT_OP ||
			op == EQUAL_OP ||
			op == SMALLER_EQ_OP ||
			op == GREATOR_EQ_OP ||
			op == SHIFT_RIGHT_OP ||
			op == ADD_ASSIGN_OP ||
			op == MINUS_ASSIGN_OP ||
			op == NOT_EQUAL_OP ||
			op == SHIFT_LEFT_OP){
		tkns->idx++;
	}
}


void get_string_value(TKNS *tkns, char buff[]){
	pass_by_type(tkns, DOUBLE_QUOTE, "Invalid string", "\"");
	while(tkns->tokens[tkns->idx].type != DOUBLE_QUOTE){
		strcat(buff, tkns->tokens[tkns->idx].word);
		tkns->idx++;
	}
	pass_by_type(tkns, DOUBLE_QUOTE, "Invalid string", "\"");
}

/* get_statement: detect: ...int a = 0;... in for loop*/
STMT get_statement(TKNS *tkns){
	STMT st;
	st.literal = 0;

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == END_SIGN){
		st.op = NO_OP;
		return st;
	}

	st.op = get_operator(tkns);

	// TODO: variable name
	pass_by_type(tkns, IDENTIFIER, "Invalid syntax", NULL);
	strcpy(st.left, tkns->tokens[tkns->idx - 1].word);
	check_name(tkns, st.left, 1);

	skip_whitespace(tkns);

	if(st.op == COMPLEMENT_OP){ return st; }

	st.op = get_operator(tkns);
	if(st.op == INVALID_OP || st.op == COMPLEMENT_OP){
		throw_err(tkns, "Invalid operator", NULL);
	} else {
		st.op = get_operator(tkns);
		if(st.op == INVALID_OP || st.op == NO_OP){
			throw_err(tkns, "Invalid operator", NULL);
		} else if(st.op == INCREMENT_OP || st.op == DECREMENT_OP){
			tkns->idx += 2;
			return st;
		} else {
			tkns->idx++;
		}
		skip_double_op(tkns, st.op);
		skip_whitespace(tkns);

		pass_by_type(tkns, INTEGER_VALUE, "Invalid syntax", "integer");
		strcpy(st.right, tkns->tokens[tkns->idx - 1].word);


		st.literal = 1;
		// **IMPORTANT**: check for different kind of names
	}

	return st;
}


/* get_qtype: get a type (with name) e.g. int a, char b, char c[] */
qvar_t get_qtype(TKNS *tkns, char name[]){
	token_t t;
	qvar_t type = QVAR_INT;

	if((t = tkns->tokens[tkns->idx].type) == INT_KEYWORD || t == CHAR_KEYWORD){
		tkns->idx++;

		skip_whitespace(tkns);

		if(tkns->tokens[tkns->idx].type == IDENTIFIER){
			strcpy(name, tkns->tokens[tkns->idx].word);
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid name", "identifier");
		}

		if(see_forward(tkns, 0, BRAKET_OPN, BRAKET_CLS)){
			type = CONSTANT_STRING;
			tkns->idx = tkns->idx + 2;

		} else if(t == INT_KEYWORD){
			type = QVAR_INT;
		} else {
			type = QVAR_CHAR;
		}
	}

	skip_whitespace(tkns);

	return type;
}

int capture_arg(TKNS *tkns, Qarg *qarg){
	token_t t;
	if((t = tkns->tokens[tkns->idx].type) == INT_KEYWORD || t == CHAR_KEYWORD){
		qarg->type = get_qtype(tkns, qarg->name);
		qarg->addr = pop_ram();
	} else {
		return 1;
	}
	return 0;
}


int pass_by_qvar(TKNS *tkns, Qvar *qvar){
	*qvar = empty_qvar();

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		if(get_qvar(tkns->tokens[tkns->idx].word, qvar)){
			throw_err(tkns, "Invlaid identifier", "valid variable name");
		}
		tkns->idx++;
		return 0;

	} else if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		if(get_literal_value(tkns->tokens[tkns->idx].word, &qvar->numeric_value)){
			throw_err(tkns, "Invlaid identifier", "valid variable name");
		}
		tkns->idx++;
		qvar->type = CONSTANT_INTEGER;
		return 0;

	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		if(get_char_value(tkns, (char *)&qvar->numeric_value)){
			throw_err(tkns, "Invlaid identifier", "valid variable name");
		}
		qvar->type = CONSTANT_CHAR;
		return 0;

	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		get_string_value(tkns, qvar->const_str);
		qvar->type = CONSTANT_STRING;
		return 0;

	} else {
		return 1;
	}

	return 0;
}






	// EQUAL_OP,           // ==
	// NOT_EQUAL_OP,       // !=
	// ASSIGN_OP,          // =
	// SMALLER_OP,         // <
	// GREATOR_OP,         // >
	// SMALLER_EQ_OP,      // <=
	// GREATOR_EQ_OP,      // >=
	// ADD_OP,             // +
	// MINUS_OP,           // -
	// INCREMENT_OP,       // ++
	// DECREMENT_OP,       // --
	// SHIFT_RIGHT_OP,     // >>
	// SHIFT_LEFT_OP,      // <<
	// COMPLEMENT_OP,      // ~
	// AND_OP,             // &
	// OR_OP,              // |
	// ADD_ASSIGN_OP,      // +=
	// MINUS_ASSIGN_OP,    // -=
	// NO_OP,              // No operator
	// INVALID_OP          // invalid(err) operator


operator capture_operator(TKNS *tkns, snip_t *st){
	if(see_forward(tkns, 0, EQUAL_SIGN, EQUAL_SIGN) ||
			see_forward(tkns, -1, EQUAL_SIGN, EQUAL_SIGN)){                // ==
		*st = CONDITIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return EQUAL_OP;

	} else if(see_forward(tkns, 0, EXCLAMATION_SIGN, EQUAL_SIGN)){   // !=
		*st = CONDITIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return NOT_EQUAL_OP;

	} else if(see_forward(tkns, 0, LEFT_SIGN, EQUAL_SIGN)){          // <=
		*st = CONDITIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return SMALLER_EQ_OP;

	} else if(see_forward(tkns, 0, RIGHT_SIGN, EQUAL_SIGN)){         // >=
		*st = CONDITIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return GREATOR_OP;

	} else if(see_forward(tkns, 0, PLUS_SIGN, EQUAL_SIGN)){          // +=
		*st = ASSIGNMENT_SNIP;
		tkns->idx = tkns->idx + 2;
		return ADD_ASSIGN_OP;

	} else if(see_forward(tkns, 0, MINUS_SIGN, EQUAL_SIGN)){         // -=
		*st = ASSIGNMENT_SNIP;
		tkns->idx = tkns->idx + 2;
		return MINUS_ASSIGN_OP;

	} else if(see_forward(tkns, 0, PLUS_SIGN, PLUS_SIGN)){           // ++
		*st = ITTERATIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return INCREMENT_OP;

	} else if(see_forward(tkns, 0, MINUS_SIGN, MINUS_SIGN)){         // --
		*st = ITTERATIONAL_SNIP;
		tkns->idx = tkns->idx + 2;
		return DECREMENT_OP;

	} else if(see_forward(tkns, 0, LEFT_SIGN, LEFT_SIGN)){           // <<
		*st = ARITHMETIC_SNIP;
		tkns->idx = tkns->idx + 2;
		return SHIFT_LEFT_OP;

	} else if(see_forward(tkns, 0, RIGHT_SIGN, RIGHT_SIGN)){         // >>
		*st = ARITHMETIC_SNIP;
		tkns->idx = tkns->idx + 2;
		return SHIFT_RIGHT_OP;

	} else if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){           // =
		*st = ASSIGNMENT_SNIP;
		tkns->idx++;
		return ASSIGN_OP;

	} else if(tkns->tokens[tkns->idx].type == LEFT_SIGN){            // <
		*st = CONDITIONAL_SNIP;
		tkns->idx++;
		return SMALLER_OP;

	} else if(tkns->tokens[tkns->idx].type == RIGHT_SIGN){           // >
		*st = CONDITIONAL_SNIP;
		tkns->idx++;
		return GREATOR_OP;

	} else if(tkns->tokens[tkns->idx].type == PLUS_SIGN){            // +
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return ADD_OP;

	} else if(tkns->tokens[tkns->idx].type == MINUS_SIGN){           // -
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return MINUS_OP;

	} else if(tkns->tokens[tkns->idx].type == TILDE_SIGN){           // ~
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return COMPLEMENT_OP;

	} else if(tkns->tokens[tkns->idx].type == AND_SIGN){             // &
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return AND_OP;

	} else if(tkns->tokens[tkns->idx].type == OR_SIGN){              // |
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return OR_OP;

	} else if(tkns->tokens[tkns->idx].type == CARET_SIGN){           // ^
		*st = ARITHMETIC_SNIP;
		tkns->idx++;
		return XOR_OP;

	}
	return INVALID_OP;
}






/* get_snippet get block of code */
SNIP get_snippet(TKNS *tkns, token_t endtok){
	SNIP snip;
	snip.op = NO_OP;                        // NO operator
	snip.left = empty_qvar();               // Empty left side
	snip.right = empty_qvar();              // Empty right side
	snip.assigned = empty_qvar();           // Empty assigned variable
	snip.type = NOT_EFFECTIVE_SNIP;         // Useless block of code (conditional, itration...)
	snip.assigne_type = NO_ASSIGNMENT_ASG;  // No varialbe placed or defined

	snip.func.arg_len = 0;
	snip.arg_len = 0;

	token_t t;

	// Check that starts with (int or char) it means it's for varialbe or function assignment
	if((t = tkns->tokens[tkns->idx].type) == INT_KEYWORD || t == CHAR_KEYWORD){

		// Get the var's name & return type (int a, char a, char a[])
		snip.assigned.type = get_qtype(tkns, snip.assigned.name);

		// If there is = means it's varialbe assignment otherwise it's for function assignment
		if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
			tkns->idx++;
			skip_whitespace(tkns);

			// Get const string (char []) value "..."
			if(snip.assigned.type == CONSTANT_STRING){
				get_string_value(tkns, snip.assigned.const_str);

			// Get (int) value 123..
			} else if (snip.assigned.type == QVAR_INT){
				if(tkns->tokens[tkns->idx].type == INTEGER_VALUE){
					if(get_literal_value(tkns->tokens[tkns->idx].word, &snip.assigned.numeric_value)){
						throw_err(tkns, "Invalid numeric value", "integer");
					}
				}
				tkns->idx++;

			// Get (char) value '.'
			} else if (snip.assigned.type == QVAR_CHAR){
				if(get_char_value(tkns, (char *)&snip.assigned.numeric_value)){
					throw_err(tkns, "Invalid character", "single character");
				}
			} else {
				throw_err(tkns, "Invalid statement", NULL);
			}
			

			pass_by_type(tkns, endtok, "Invalid syntax", "EOF");


			snip.assigned.addr = pop_ram();
			snip.assigne_type = VARIABLE_ASSIGNMENT_ASG;


			if(save_qvar(snip.assigned, GLOBAL_LOCAL_STACK)){
				throw_err(tkns, "Varialbe already exists", NULL);
			}

		snip.type = ASSIGNMENT_SNIP;

		} else if (tkns->tokens[tkns->idx].type == PAREN_OPN){
			tkns->idx++;
			while(tkns->tokens[tkns->idx].type != PAREN_CLS){
				skip_whitespace(tkns);
				int success = capture_arg(tkns, &snip.func.args[snip.func.arg_len]) == 0;
				if(success){

					// Check that every argument's name is unique
					if(arg_exists(snip.func.args, snip.func.arg_len, snip.func.args[snip.func.arg_len])){
						throw_err(tkns, "Argument with this name already exists", NULL);
					}

					snip.func.arg_len++;
					if(tkns->tokens[tkns->idx].type == COMMA_SIGN){ tkns->idx++; continue; }
				} else {
					if(tkns->tokens[tkns->idx].type == PAREN_CLS ||
							tkns->tokens[tkns->idx].type == COMMA_SIGN){ tkns->idx++; continue; }
					throw_err(tkns, "Invlaid function argument", NULL);
				}
			}

			tkns->idx++;  // SKIP  ')'
			skip_whitespace(tkns);

			pass_by_type(tkns, BRACE_OPN, "Invalid syntax", "{");

			static TKNS save;
			get_brace_content(tkns, &snip.func.body);
			tkns->idx++;
			strcpy(snip.func.name, snip.assigned.name);
			snip.func.return_type = snip.assigned.type;
			snip.assigne_type = FUNCTION_ASSIGNMENT_ASG;


			int result = save_qfunc(snip.func);
			if(result == 1){
				throw_err(tkns, "Function already exists", NULL);
			} else if(result == 2) {
				throw_err(tkns, "A variable with this name already exists", NULL);
			}

		}


		snip.type = ASSIGNMENT_SNIP;

	} else if(tkns->tokens[tkns->idx].type == HASHTAG){
		tkns->idx++;

		pass_by_type(tkns, DEFINE_KEYWORD, "Invalid syntax", "define");
		skip_whitespace(tkns);

		pass_by_type(tkns, IDENTIFIER, "Invalid define", "Valid name");
		strcpy(snip.assigned.name, tkns->tokens[tkns->idx - 1].word);
		skip_whitespace(tkns);

		pass_by_type(tkns, INTEGER_VALUE, "Invalid define", "integer");
		if(get_literal_value(tkns->tokens[tkns->idx - 1].word, &snip.assigned.addr)){
			throw_err(tkns, "Invalid define value", "valid integer");
		}
		snip.assigned.numeric_value = snip.assigned.addr;
		snip.assigned.type = QVAR_DEFINE;
		snip.type = ASSIGNMENT_SNIP;

		snip.assigne_type = MACRO_ASSIGNMENT_ASG;
	}

	if(snip.type == ASSIGNMENT_SNIP){ return snip; }


	// Check for function call
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		if(get_qfunc(tkns->tokens[tkns->idx].word, &snip.func) == 0){
			tkns->idx++;

			skip_whitespace(tkns);
			pass_by_type(tkns, PAREN_OPN, "Invalid function call", "'('");
			skip_whitespace(tkns);

			// Get Function Arguments
			if(snip.func.arg_len != 0){
				for(int a = 0; a < snip.func.arg_len; ++a){
					Qvar q;

					if(pass_by_qvar(tkns, &q)){
						throw_err(tkns, "Invalid function argument", NULL);
					}

					if(same_type_arg(q, snip.func.args[a]) == 0){
						throw_err(tkns, "Invalid argument type", NULL);
					}

					// matched
					snip.args[snip.arg_len++] = q;

					if(a != snip.func.arg_len - 1){
						skip_whitespace(tkns);
						pass_by_type(tkns, COMMA_SIGN, "Invalid function argument", ",");
						skip_whitespace(tkns);
					}
				}
			}

			skip_whitespace(tkns);
			pass_by_type(tkns, PAREN_CLS, "Invalid function call", "')'");
			skip_whitespace(tkns);
			pass_by_type(tkns, endtok, "Invalid expression", "EOF");
			snip.type = FUNCTION_CALL_SNIP;
			return snip;
		}
	}


	pass_by_qvar(tkns, &snip.left);
	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		copy_qvar(snip.assigned, snip.left);
		snip.left = empty_qvar();  // Clear 'snip.left'
		snip.assigne_type = UPDATE_AST;

		tkns->idx++;

		skip_whitespace(tkns);
		pass_by_qvar(tkns, &snip.left);
		skip_whitespace(tkns);

		snip.op = capture_operator(tkns, &snip.type);
		skip_whitespace(tkns);
	
		if(snip.type == ITTERATIONAL_SNIP){
			pass_by_type(tkns, endtok, "Invalid expression", "EOF");
			return snip;
		}


		pass_by_qvar(tkns, &snip.right);

	} else {

		skip_whitespace(tkns);
		snip.op = capture_operator(tkns, &snip.type);
		skip_whitespace(tkns);

		if(snip.type == ITTERATIONAL_SNIP){
			pass_by_type(tkns, endtok, "Invalid expression", "EOF");
			return snip;
		}


		pass_by_qvar(tkns, &snip.right);
	}

	skip_whitespace(tkns);
	pass_by_type(tkns, endtok, "Invalid expression", "EOF");

	return snip;
}


Qfor for_asgmt(TKNS *tkns){
	Qfor qfor;
	tkns->idx++;
	skip_whitespace(tkns);

	pass_by_type(tkns, PAREN_OPN, "Invalid for loop", "'('");
	skip_whitespace(tkns);

	qfor.init = get_snippet(tkns, END_SIGN);
	skip_whitespace(tkns);
	if(qfor.init.type != ASSIGNMENT_SNIP && qfor.init.type != NOT_EFFECTIVE_SNIP){
		throw_err(tkns, "Invalid conditional statement", NULL);
	}

	qfor.cond = get_snippet(tkns, END_SIGN);
	skip_whitespace(tkns);
	if(qfor.cond.type != CONDITIONAL_SNIP && qfor.cond.type != NOT_EFFECTIVE_SNIP){
		throw_err(tkns, "Invalid conditional statement", NULL);
	}

	qfor.iter = get_snippet(tkns, PAREN_CLS);
	skip_whitespace(tkns);
	if(qfor.iter.type != ITTERATIONAL_SNIP && qfor.iter.type != NOT_EFFECTIVE_SNIP){
		throw_err(tkns, "Invalid conditional statement", NULL);
	}

	pass_by_type(tkns, BRACE_OPN, "Invalid syntax", "{");
	get_brace_content(tkns, &qfor.body);
	tkns->idx++;

	return qfor;
}


/* parse: while(...condition...){ ... }*/
Qwhile while_asgmt(TKNS *tkns){
	Qwhile qwhile;
	tkns->idx++;
	skip_whitespace(tkns);

	pass_by_type(tkns, PAREN_OPN, "Invalid while loop", "'('");
	skip_whitespace(tkns);

	qwhile.cond = get_snippet(tkns, PAREN_CLS);
	skip_whitespace(tkns);

	if(qwhile.cond.type != CONDITIONAL_SNIP){
		printf("%d\n", qwhile.cond.type);
		throw_err(tkns, "Invalid conditional statement", NULL);
	}

	pass_by_type(tkns, BRACE_OPN, "Invalid syntax", "{");
	get_brace_content(tkns, &qwhile.body);
	tkns->idx++;

	return qwhile;
}


Qif if_asgmt(TKNS *tkns){
	Qif qif;
	qif.contains_else = 0;
	tkns->idx++;
	skip_whitespace(tkns);

	pass_by_type(tkns, PAREN_OPN, "Invalid if", "'('");
	skip_whitespace(tkns);

	qif.cond = get_snippet(tkns, PAREN_CLS);
	skip_whitespace(tkns);

	if(qif.cond.type != CONDITIONAL_SNIP){
		printf("%d\n", qif.cond.type);
		throw_err(tkns, "Invalid conditional statement", NULL);
	}

	pass_by_type(tkns, BRACE_OPN, "Invalid syntax", "{");
	get_brace_content(tkns, &qif.if_body);
	tkns->idx++;
	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == ELSE_KEYWORD){
		qif.contains_else = 1;
		tkns->idx++;
		skip_whitespace(tkns);

		pass_by_type(tkns, BRACE_OPN, "Invalid syntax", "{");
		get_brace_content(tkns, &qif.else_body);
		tkns->idx++;
	}

	return qif;
}

// FOR_ASGMT for_asgmt(TKNS *tkns){
// 	FOR_ASGMT fr;
// 	tkns->idx++;
// 	skip_whitespace(tkns);
// 
// 	// initial
// 	pass_by_type(tkns, PAREN_OPN, "Invalid character", "'('");
// 	skip_whitespace(tkns);
// 	fr.init = get_statement(tkns);
// 
// 	skip_whitespace(tkns);
// 
// 	// condition
// 	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
// 	skip_whitespace(tkns);
// 	fr.cond = get_statement(tkns);
// 
// 	skip_whitespace(tkns);
// 
// 	// iterator
// 	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
// 	skip_whitespace(tkns);
// 
// 	fr.iter = get_statement(tkns);
// 
// 	skip_whitespace(tkns);
// 
// 	// end ')'
// 	pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");
// 	skip_whitespace(tkns);
// 	// body
// 	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
// 	get_brace_content(tkns, &fr.body);  // get everything in for loop as tokens
// 	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");
// 	// tkns->idx++;  // ?
// 	return fr;
// }



// void get_cond(TKNS *tkns, TKNS *dst){
// 	skip_whitespace(tkns);
// 	pass_by_type(tkns, PAREN_OPN, "Invalid syntax", "(");
// 	while(tkns->tokens[tkns->idx].type != PAREN_CLS)
// 	pass_by_type(tkns, PAREN_CLS, "Invalid syntax", ")");
// }

BODY_ASGMT body_asgmt(TKNS *tkns, body_t type){
	BODY_ASGMT wa;
	wa.type = type;
	// wa.cond.op = NO_OP;
	tkns->idx++;
	skip_whitespace(tkns);

	// (...), condition (test) part of the while loop
	pass_by_type(tkns, PAREN_OPN, "Invalid character", "'('");
	skip_whitespace(tkns);
	// int tidx = tkns->id;
	wa.cond = get_statement(tkns);
	// tkns->idx = tidx;
	// get_paren_content(tkns, &wa.cond.body);

	skip_whitespace(tkns);
	pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");

	skip_whitespace(tkns);

	// body of the loop
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");

	get_brace_content(tkns, &wa.body);
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	// **DEBUG**
	// printf("BODY-------------------------------------->\n");
	// for(int i = 0; i < wa.body.max; i++){
	// 	printf("%s\n", wa.body.tokens[i].word);
	// }
	// printf("<--------------------------------------BODY\n");

	return wa;
}


BODY_ASGMT else_asgmt(TKNS *tkns){
	BODY_ASGMT wa;
	wa.type = ELSE_BODY;
	wa.cond.op = NO_OP;
	tkns->idx++;

	skip_whitespace(tkns);

	// body of the loop
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	get_brace_content(tkns, &wa.body);
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	return wa;
}


CNST_VAR return_asgmt(TKNS *tkns, var_t return_type){
	tkns->idx++;
	skip_whitespace(tkns);
	CNST_VAR vl;

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		if(variable_exits(tkns->tokens[tkns->idx].word, &vl) == 0){
			throw_err(tkns, "Variable not found", NULL);
		}
		if(vl.type != return_type){
			char type[20];
			type_to_str(return_type, type);
			throw_err(tkns, "Invalid return type", type);
		}
		tkns->idx++;
	} else {
		vl = const_var(tkns);
	}

	if(vl.type == STR_VAR){
		throw_err(tkns, "Unsupported return type", "int, char");
	}

	if(vl.type != return_type){
		char type[20];
		type_to_str(return_type, type);
		throw_err(tkns, "Invalid return type", type);
	}
	skip_whitespace(tkns);
	pass_by_type(tkns, END_SIGN, "Invalid syntax", ";");
	return vl;
}



side_t get_side(TKNS *tkns, token_t split){
	side_t side;
	side.value = 0;
	side.arithmetic = 0;
	side.complement = 0;

	if(tkns->tokens[tkns->idx].type == CARET_SIGN){
		side.complement = 1;
		tkns->idx++;
	}

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		CNST_VAR var;
		if(variable_exits(tkns->tokens[tkns->idx].word, &var) == 0){
			tkns->idx++;
			throw_err(tkns, "variable not exits in this expression", NULL);
		}
		side.value = var.int_value;
		side.var = var;
		tkns->idx++;

	} else if (tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		if(get_literal_value(tkns->tokens[tkns->idx].word, &side.value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid value", "int (0 to 255)");
		}
	} else {
		if(tkns->tokens[tkns->idx].type != split){
			throw_err(tkns, "Invalid expression value", NULL);
		}
	}

	// end of the expression
	if(tkns->tokens[tkns->idx].type == split){ return side; }
	side.arithmetic = get_arighmetic(tkns);

	return side;
}


CNST_VAR skip_by_arg(TKNS *tkns, ARG arg){
	CNST_VAR var;

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		if(variable_exits(tkns->tokens[tkns->idx].word, &var) == 0){
			throw_err(tkns, "Variable not found", NULL);
		}
		tkns->idx++;
	} else {
		var = const_var(tkns);
	}

	if(var.type != arg.type){
		char type[20];
		type_to_str(arg.type, type);
		throw_err(tkns, "Invalid type", type);
	}
	return var;
}


int function_call(TKNS *tkns, func_t *func, CNST_VAR *arguments, int *idx){
	int tmpidx = tkns->idx;
	// int idx = 0;

	skip_whitespace(tkns);
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){

		if(get_func(tkns->tokens[tkns->idx].word, func)){
			tkns->idx++;
			skip_whitespace(tkns);

			pass_by_type(tkns, PAREN_OPN, "Invalid syntax", "'('");
			// tkns->idx++;
			if(func->arg_len != 0){

				for(int i = 0; i < func->arg_len; ++i){
					CNST_VAR avar = skip_by_arg(tkns, func->args[i]);
					// func->args[i].addr = pop_ram();
					func->args[i].addr = 0; 
					// printf("POPED ADDRESS: %d\n", func->args[i].addr);
					arguments[*idx] = avar;
					*idx = *idx + 1;
					// printf("IDX: %d\n", idx);

					skip_whitespace(tkns);
					if(i == func->arg_len - 1){
						pass_by_type(tkns, PAREN_CLS, "Invalid syntax", "')'");
						break;
					} else {
						pass_by_type(tkns, COMMA_SIGN, "Invalid syntax", ",");
					}
				}
			}
			return 1;
		} else { tkns->idx = tmpidx; return 0; }
	} else { tkns->idx = tmpidx; return 0; }
}


side_t empty_side(){
	side_t side;
	side.value = 0;
	side.complement = 0;
	side.arithmetic = 0;
	side.var.int_value = 0;
	return side;
}

EXPR get_expr(TKNS *tkns, token_t endtok){
	EXPR expr;

	// Function
	expr.type = EXPR_EMPTY;
	expr.caller.arg_len = 0;
	expr.caller.return_type = INT_VAR;

	expr.op = NO_OP;
	expr.args_len = 0;
	expr.mono_side = 1;
	expr.left = empty_side();
	expr.right = empty_side();

	if(function_call(tkns, &expr.caller, expr.args, &expr.args_len)){
		expr.type = EXPR_FUNCTION_CALL;
		return expr;
	}


	
	skip_whitespace(tkns);

	int tmpidx = tkns->idx;
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		CNST_VAR var;
		if(variable_exits(tkns->tokens[tkns->idx].word, &var) == 0){
			tkns->idx++;
			throw_err(tkns, "variable not exits", NULL);
		}
		strcpy(expr.assign_name, tkns->tokens[tkns->idx].word);
		tkns->idx++;

		skip_whitespace(tkns);
		if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
			tkns->idx++;
			expr.type = EXPR_ASSIGNABLE;
			expr.mono_side = 0;
		} else {
			memset(expr.assign_name, '\0', sizeof(expr.assign_name));
			expr.type = EXPR_EMPTY;
			tkns->idx = tmpidx;
		}
	}






	skip_whitespace(tkns);

	expr.left = get_side(tkns, endtok);

	skip_whitespace(tkns);

	if(tkns->tokens[tkns->idx].type == endtok){ expr.mono_side = 0; return expr; }
	

	skip_whitespace(tkns);


	expr.op = get_operator(tkns);

	if(expr.op == INVALID_OP || expr.op == NO_OP){ throw_err(tkns, "Invalid operator", NULL); }

	tkns->idx++;
	skip_double_op(tkns, expr.op);
	skip_whitespace(tkns);

	expr.right = get_side(tkns, endtok);

	if(tkns->tokens[tkns->idx].type == endtok){
		return expr;
	} else { throw_err(tkns, "Invalid expression", NULL); }

	return expr;
}



CNST_VAR get_value(TKNS *tkns){
	CNST_VAR var;
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		if(variable_exits(tkns->tokens[tkns->idx].word, &var) == 0){
			throw_err(tkns, "Variable not found", NULL);
		}
		tkns->idx++;
	} else {
		var = const_var(tkns);
	}
	return var;
}



ARG get_arg_struct(func_t f, char name[]){
	for(int i = 0; i < f.arg_len; ++i){
		if(strcmp(f.args[i].name, name) == 0){
			return f.args[i];
		}
	}
	return (ARG){.addr = 0, .type = INT_VAR};
}



/* emit_stmt: code emission for statement (check) */
void emit_stmt(char dst[], STMT stmt, int reverse){

	if(stmt.op == EQUAL_OP || stmt.op == NOT_EQUAL_OP){
		strcatf(dst, "\tMOVF %s, 0\n", stmt.left); 
		if(stmt.literal){
			strcatf(dst, "\tXORLW %s\n", stmt.right);
		} else {
			strcatf(dst, "\tXORWF %s, 0\n", stmt.right);
		}
		if(stmt.op == NOT_EQUAL_OP){
			if(reverse){
				strcatf(dst, "\tBTFSS STATUS, Z");
			} else {
				strcatf(dst, "\tBTFSC STATUS, Z");
			}
		} else {
			if(reverse){
				strcatf(dst, "\tBTFSC STATUS, Z");
			} else {
				strcatf(dst, "\tBTFSS STATUS, Z");
			}
		}
	} else if (stmt.op == GREATOR_EQ_OP || stmt.op == SMALLER_EQ_OP){
		if(stmt.literal){
			strcatf(dst, "\tMOVLW %s\n", stmt.right); 
			strcatf(dst, "\tSUBWF %s, 0\n", stmt.left);
		} else {
			strcatf(dst, "\tMOVF %s, 0\n", stmt.left); 
			strcatf(dst, "\tSUBWF %s, 0\n", stmt.right);
		}

		// Auto reverse (because **SUBWF** can't accept literal so we give it to W and make a reverse condition)
		if(stmt.literal){ reverse = reverse == 0; }

		if(stmt.op == SMALLER_EQ_OP){
			if(reverse){
				strcatf(dst, "\tBTFSS STATUS, C");
			} else {
				strcatf(dst, "\tBTFSC STATUS, C");
			}
		} else {
			if(reverse){
				strcatf(dst, "\tBTFSC STATUS, C");
			} else {
				strcatf(dst, "\tBTFSS STATUS, C");
			}
		}

	}

}

