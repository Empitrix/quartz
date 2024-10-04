#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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
		if(tkns->tokens[tkns->idx].type == IDENTIFIER && get_literal_value(tkns->tokens[tkns->idx].word, value) == 0){
			tkns->idx++;
		} else {
			throw_err(tkns, "invalid value", "literal value"); exit(0);
		}
	} else {
		get_char_value(tkns, value);
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

	} else if(strcmp(tkns->tokens[tkns->idx].word, "(") == 0){
		// Function detected

	} else if(strcmp(tkns->tokens[tkns->idx].word, "=") == 0){
		// Variable detected
		u8_var_asgmt(tkns, &ae.value, ae.type);
		printf("VALUE: >%d<\n", ae.value);
		return ae;

	} else {
		// Variable detected
		throw_err(tkns, "Invalid symbol", NULL);
		exit(0);
	}

	return ae;
}
