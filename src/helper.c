#include "types.h"
#include "utility.h"
#include "global.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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



/* get_brace_content: get everything between two '{' and '}' (braces are not part of the extraction)*/
void get_brace_content(TKNS *tkns, TKNS *save){
	int open;
	open = 0;
	save->idx = 0;
	save->max = 0;
	int sidx = tkns->idx;

	int in_quote = 0;

	while(tkns->tokens[tkns->idx].type != BRACE_CLS || open != 0){

		// Detect qoute (toggle)
		if(tkns->tokens[tkns->idx].type == SINGLE_QUOTE){ in_quote = in_quote == 0; }

		// Detect "{" or "}" if there is no qoute (')
		if(in_quote == 0){
			if(tkns->tokens[tkns->idx].type == BRACE_OPN){ open++; }
			if(tkns->tokens[tkns->idx].type == BRACE_CLS){ open--; }
		}

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




void get_string_value(TKNS *tkns, char buff[]){
	pass_by_type(tkns, DOUBLE_QUOTE, "Invalid string", "\"");
	while(tkns->tokens[tkns->idx].type != DOUBLE_QUOTE){
		strcat(buff, tkns->tokens[tkns->idx].word);
		tkns->idx++;
	}
	pass_by_type(tkns, DOUBLE_QUOTE, "Invalid string", "\"");
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

		if(save_qvar(snip.assigned, GLOBAL_LOCAL_STACK)){
			throw_err(tkns, "Varialbe already exists", NULL);
		}
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



Qvar handle_return(TKNS *tkns){
	Qvar q = empty_qvar();
	tkns->idx++;  // Skip "RETURN" keyword
	skip_whitespace(tkns);
	pass_by_qvar(tkns, &q);
	skip_whitespace(tkns);
	pass_by_type(tkns, END_SIGN, "Invalid return statement", ";");
	return q;
}


/* Handle Raw Assembly code e.g. `MOVLW 'A' `*/
void handle_rasm(TKNS *tkns, char buff[]){
	tkns->idx++;

	int quote = 0;
	int brace = 0;

	while(tkns->tokens[tkns->idx].type != BACKTICK_SIGN){

		// Toggle qoute
		if(tkns->tokens[tkns->idx].type == SINGLE_QUOTE){ quote = quote == 0; }

		if(tkns->tokens[tkns->idx].type == BRACE_OPN && quote == 0){
			brace = 1;
			tkns->idx++;
			continue;
		}

		if(tkns->tokens[tkns->idx].type == BRACE_CLS && quote == 0){
			brace = 0;
			tkns->idx++;
			continue;
		}
		
		// Skip whitespaces between {...}
		if(brace && tkns->tokens[tkns->idx].type == WHITESPACE){
			tkns->idx++;
			continue;
		}

		if(brace && tkns->tokens[tkns->idx].type != WHITESPACE){
			Qvar q;

			if(pass_by_qvar(tkns, &q)){
				throw_err(tkns, "Invalid assembly syntax", "");
			}
			if(qvar_defined(&q)){
				strcatf(buff, "0x%x", q.addr);
			} else {
				strcatf(buff, "0x%x", q.numeric_value);
			}

			continue;
		}

		strcatf(buff, "%s", tkns->tokens[tkns->idx].word);


		if(tkns->tokens[tkns->idx].type == NEWLINE){
			throw_err(tkns, "Assembly code must be in a single line", "");
		}

		tkns->idx++;
	}

	skip_whitespace(tkns);
	pass_by_type(tkns, BACKTICK_SIGN, "Invalid assembly syntax", "`");
	skip_whitespace(tkns);
	pass_by_type(tkns, END_SIGN, "Invalid assembly syntax", ";");
}
