#include "types.h"
#include "utility.h"
#include "global.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* pass_by_type: Check if the token has the given type; if it does, move to the next one; otherwise, throw an error. */
void pass_by_type(TKNS *tkns, token_t type, const char *msg, const char *exp){
	if(tkns->tokens[tkns->idx].type == type){
		tkns->idx++;
	} else {
		throw_err(tkns, msg, exp);
		exit(0);
	}
}


/* skip_white_space: skip white space */
void skip_white_space(TKNS *tkns){
	if(tkns->tokens[tkns->idx].type == WHITESPACE){ tkns->idx++; }
}

/* skip_white_space: skip white space + Newline */
void skip_gap(TKNS *tkns){
	while (tkns->tokens[tkns->idx].type == WHITESPACE || tkns->tokens[tkns->idx].type == NEWLINE) {
		tkns->idx++;
	}
}


var_t get_type(TKNS *tkns){
	var_t v;
	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == INT_KEYWORD){
		tkns->idx++;
		v = INT_VAR;
		// int
		return v;
	}

	if(tkns->tokens[tkns->idx].type == CHAR_KEYWORD){
		tkns->idx++;
		v = CHAR_VAR;
		skip_white_space(tkns);

		if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
			tkns->idx++;
			skip_white_space(tkns);
			pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
			v = STR_VAR;
			// str
			return v;
		} else {
			// char
			return v;
		}
		
	} else {
		throw_err(tkns, "Invalid type", "int, char, char[]");
		exit(0);
	}

	return v;
}


int contains_str_type(TKNS *tkns, var_t *type){
	if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
		tkns->idx++;
		skip_white_space(tkns);
		pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
		skip_white_space(tkns);
		*type = STR_VAR;
		return 1;
	}
	return 0;
}


// /* cet_var_arg: get function arguments */
// int get_var_arg(TKNS *tkns, char name[], var_t *type){
// 
// 	skip_white_space(tkns);
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
// 		skip_white_space(tkns);
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
// 		skip_white_space(tkns);
// 
// 
// 		if(tkns->tokens[tkns->idx].type == COMMA_SIGN){
// 			return 0;
// 		}
// 
// 		if(tkns->tokens[tkns->idx].type == BRAKET_OPN){
// 			tkns->idx++;
// 			skip_white_space(tkns);
// 			pass_by_type(tkns, BRAKET_CLS, "Invalid syntax", "']'");
// 			skip_white_space(tkns);
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

		skip_white_space(tkns);

		args[*arg_len].type = get_type(tkns);
		skip_white_space(tkns);

		pass_by_type(tkns, IDENTIFIER, "Invalid name", NULL);
		strcpy(args[*arg_len].name, tkns->tokens[tkns->idx - 1].word);
		update_name_state(tkns, args[*arg_len].name, 1);
		skip_white_space(tkns);

		contains_str_type(tkns, &args[*arg_len].type);

		*arg_len = *arg_len + 1;

		if(tkns->tokens[tkns->idx].type == COMMA_SIGN){
			tkns->idx++;
			continue;
		} else if (tkns->tokens[tkns->idx].type == PAREN_CLS){
			break;
		} else {
			throw_err(tkns, "Invalid syntax", ", or )");
			exit(0);
		}

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
			exit(0);

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
				throw_err(tkns, "Invalid value", "literal value"); exit(0);
			}
			return -1;
		}
	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		cvar->type = CHAR_VAR;
		if(get_char_value(tkns, &cvar->char_value)){
			return -1;
			if(force){
				throw_err(tkns, "Invalid character", NULL); exit(0);
			}
		}
	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		cvar->type = STR_VAR;
		if(get_string(tkns, cvar->str_value) == 0){
			tkns->idx++;
			return 1;
		} else {
			if(force){
				throw_err(tkns, "Invalid string constant", NULL); exit(0);
			}
			return -1;
		}
	} else {
		if(force){
			throw_err(tkns, "Invalid constant", NULL);
			exit(0);
		}
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
			throw_err(tkns, "Invalid value", "literal value"); exit(0);
		}
	} else if (tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		cvar.type = CHAR_VAR;
		if(get_char_value(tkns, &cvar.char_value)){
			throw_err(tkns, "Invalid character", NULL); exit(0);
		}
	} else if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		cvar.type = STR_VAR;
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
void u8_var_asgmt(TKNS *tkns, int *value, var_t type){

	if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid symbol", "="); exit(0);
	}

	skip_white_space(tkns);

	if(type == INT_VAR){
		if(tkns->tokens[tkns->idx].type == INTEGER_VALUE && get_literal_value(tkns->tokens[tkns->idx].word, value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "invalid value", "literal value"); exit(0);
		}
	} else {
		// printf("SHOULD OF BEEN CHAR\n");
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
	ae.value = 0;
	ae.type = INT_VAR;
	ae.func.arg_len = 0;
	ae.func.return_type = INT_VAR;

	skip_white_space(tkns);
	ae.type = get_type(tkns);
	skip_white_space(tkns);

	// get varialbe name
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		strcpy(ae.name, tkns->tokens[tkns->idx].word);
		update_name_state(tkns, ae.name, 0);
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid varialbe name", NULL); exit(0);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == BRAKET_OPN){  // String detected
		ae.type = STR_VAR;
		str_var_asgmt(tkns, ae.str);
		ae.is_str = 1;
		return ae;

	} else if(tkns->tokens[tkns->idx].type == PAREN_OPN){ // Function detected
		ae.is_func = 1;
		tkns->idx++;
		get_arg(tkns, ae.func.args, &ae.func.arg_len);
		pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");
		skip_white_space(tkns);
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

	} else {  // Invalid
		throw_err(tkns, "Invalid symbol", NULL);
		exit(0);
	}

	return ae;
}


/* macro_asgmt: assignment for #include and #define */
MACRO macro_asgmt(TKNS *tkns){
	MACRO mcro;
	mcro.value.type = INT_VAR;
	mcro.type = DEFINE_MACRO;

	tkns->idx++;


	if(tkns->tokens[tkns->idx].type == INCLUDE_KEWORD || tkns->tokens[tkns->idx].type == DEFINE_KEWORD){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid preprocessor directive", "include or define");
		exit(0);
	}

	int is_include = tkns->tokens[tkns->idx - 1].type == INCLUDE_KEWORD;

	skip_white_space(tkns);

	if(is_include){
		mcro.type = INCLUDE_MACRO;


		if (tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
			if(get_string(tkns, mcro.name) == 0){
				tkns->idx++;
			} else {
				throw_err(tkns, "Invalid string constant", NULL);
				exit(0);
			}
		} else {
			throw_err(tkns, "Invalid constant", NULL);
			exit(0);
		}

	} else {

		if(tkns->tokens[tkns->idx].type == IDENTIFIER){
			strcpy(mcro.name, tkns->tokens[tkns->idx].word);

			update_name_state(tkns, mcro.name, 0);

			tkns->idx++;
		} else {
			throw_err(tkns, "A name required", NULL);
			exit(0);
		}

		skip_white_space(tkns);
		mcro.value = const_var(tkns);
	}

	skip_white_space(tkns);

	if(tkns->tokens[tkns->idx].type == NEWLINE){
		return mcro;
	} else {
		throw_err(tkns, "Invalid macro", NULL);
		exit(0);
	}

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
			// op = set_double_op(tkns, PLUS_SIGN, ADD_OP, INCREMENT_OP);
			return ADD_OP;
			break;

		case MINUS_SIGN:
			// op = set_double_op(tkns, MINUS_SIGN, MINUS_OP, DECREMENT_OP);
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
			op == SHIFT_LEFT_OP){
		tkns->idx++;
	}
}

/* get_statement: detect: ...int a = 0;... in for loop*/
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
	check_name(tkns, st.left, 1);

	skip_white_space(tkns);

	if(st.op == COMPLEMENT_OP){ return st; }

	st.op = get_operator(tkns);
	if(st.op == INVALID_OP || st.op == COMPLEMENT_OP){
		throw_err(tkns, "Invalid operator", NULL);
		exit(0);
	} else {
		st.op = get_operator(tkns);
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
		// **IMPORTANT**: check for different kind of names
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

	// end ')'
	pass_by_type(tkns, PAREN_CLS, "Invalid character", "')'");
	skip_white_space(tkns);
	// body
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	get_brace_content(tkns, &fr.body);  // get everything in for loop as tokens
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	// **DEBUG**
	// for(int i = 0; i < fr.body.max; i++){
	// 	printf("%s\n", fr.body.tokens[i].word);
	// }

	// tkns->idx++;  // ?
	return fr;
}


BODY_ASGMT body_asgmt(TKNS *tkns, body_t type){
	BODY_ASGMT wa;
	wa.type = type;
	// wa.cond.op = NO_OP;
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

	skip_white_space(tkns);

	// body of the loop
	pass_by_type(tkns, BRACE_OPN, "Invalid character", "'{'");
	get_brace_content(tkns, &wa.body);
	pass_by_type(tkns, BRACE_CLS, "Invalid character", "'}'");

	return wa;
}


CNST_VAR return_asgmt(TKNS *tkns, var_t return_type){
	tkns->idx++;
	skip_white_space(tkns);
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
		exit(0);
	}
	skip_white_space(tkns);
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
			exit(0);
		}
		side.value = var.int_value;
		tkns->idx++;

	} else if (tkns->tokens[tkns->idx].type == INTEGER_VALUE){
		if(get_literal_value(tkns->tokens[tkns->idx].word, &side.value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid value", "int (0 to 255)");
			exit(0);
		}
	} else {
		throw_err(tkns, "Invalid expression value", NULL);
		exit(0);
	}

	// end of the expression
	// if(tkns->tokens[tkns->idx].type == PAREN_CLS || tkns->tokens[tkns->idx].type == END_SIGN){
	if(tkns->tokens[tkns->idx].type == split){
		return side;
	}
	side.arithmetic = get_arighmetic(tkns);

	return side;
}


EXPR get_expr(TKNS *tkns, token_t endtok){
	EXPR expr;
	
	skip_white_space(tkns);

	int tmpidx = tkns->idx;
	if(tkns->tokens[tkns->idx].type == IDENTIFIER){
		CNST_VAR var;
		if(variable_exits(tkns->tokens[tkns->idx].word, &var) == 0){
			tkns->idx++;
			throw_err(tkns, "variable not exits", NULL);
			exit(0);
		}
		strcpy(expr.assign_name, tkns->tokens[tkns->idx].word);
		tkns->idx++;

		skip_white_space(tkns);
		if(tkns->tokens[tkns->idx].type == EQUAL_SIGN){
			tkns->idx++;
			expr.is_assign = 1;
		} else {
			memset(expr.assign_name, '\0', sizeof(expr.assign_name));
			expr.is_assign = 0;
			tkns->idx = tmpidx;
		}
	}



	skip_white_space(tkns);

	expr.left = get_side(tkns, endtok);

	skip_white_space(tkns);


	if(tkns->tokens[tkns->idx].type == endtok){ return expr; }
	
	skip_white_space(tkns);


	expr.op = get_operator(tkns);
	if(expr.op == INVALID_OP || expr.op == NO_OP){
		throw_err(tkns, "Invalid operator", NULL);
		exit(0);
	}

	tkns->idx++;
	skip_double_op(tkns, expr.op);
	skip_white_space(tkns);

	expr.right = get_side(tkns, endtok);

	if(tkns->tokens[tkns->idx].type == endtok){
		return expr;
	} else {
		throw_err(tkns, "Invalid expression", NULL);
		exit(0);
	}

	return expr;
}

