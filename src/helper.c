#include "types.h"
#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


variable_t get_type(char *type){
	if(strcmp(type, "int") == 0){
		return INT_TYPE;
	} else if (strcmp(type, "char") == 0){
		return CHAR_TYPE;
	} else {
		return INVALID_TYPE;
	}
}


// ASSIGNMENT_EXP get_assignment_exp(Token *tokens, int max, int cidx){
// 	ASSIGNMENT_EXP ae;
// 	int i;
// 	ae.type = INVALID_EXP;
// 	ae.name = NULL;
// 	for(i = cidx; i < max; ++i){
// 		if(tokens[i].type == WHITESPACE_TOK){ continue; }
// 
// 		if(i == cidx){
// 			ae.type = get_type(tokens[i].word);
// 			if(ae.type == INVALID_EXP){
// 				printf("Invalid Type\n");
// 				exit(0);
// 			}
// 			continue;
// 		}
// 
// 		// printf("%s\n", tokens[i].word);
// 
// 		if(ae.type != INVALID_EXP && tokens[i].type == IDENTIFIER_TOK && ae.name == NULL){
// 			ae.name = (char *)calloc((int)strlen(tokens[i].word) + 1, sizeof(char));
// 			strcpy(ae.name, tokens[i].word);
// 			printf("name: %s\n", ae.name);
// 			get_err_line(tokens, max, i);
// 			continue;
// 		}
// 
// 
// 		int eq_symbol = 0;
// 		int end_symbol = 0;
// 		if(ae.name != NULL && ae.type != INVALID_EXP){
// 			while(i < max){
// 				if(tokens[i].type == WHITESPACE_TOK){ ++i; continue; }
// 				if(eq_symbol == 0){
// 					if(strcmp(tokens[i].word, "=") && tokens[i].type == SYMBOL_TOK){
// 						eq_symbol = 1;
// 					}
// 					++i;
// 					continue;
// 				}
// 
// 				if(eq_symbol && tokens[i].type == IDENTIFIER_TOK){
// 					ae.value = atoi(tokens[i].word);
// 					printf("value: %d\n", ae.value);
// 				}
// 
// 				++i;
// 			}
// 		}
// 
// 
// 	}
// 	return ae;
// }


void skip_white_space(TOKEN *tokens, int *i){
	if(tokens[*i].type == WHITESPACE){ *i = *i + 1; }
}


/* start's from [... => ] = "..."; */
int str_var_asgmt(TOKEN *tokens, int max, int i, char *value){
	// string
	++i;
	skip_white_space(tokens, &i);

	if(tokens[i].type == BRAKET_CLS){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", "]");
		exit(0);
	}

	skip_white_space(tokens, &i);

	if(tokens[i].type == EQUAL_SIGN){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", "=");
		exit(0);
	}


	skip_white_space(tokens, &i);

	if(get_string(tokens, max, i, value, &i) == 0){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", NULL);
		exit(0);
	}

	skip_white_space(tokens, &i);

	if(tokens[i].type == END_SIGN){
		++i;
		return i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", ";");
		exit(0);
	}
}

int u8_var_asgmt(TOKEN *tokens, int max, int i, int *value, variable_t type){

	if(tokens[i].type == EQUAL_SIGN){
		i++;
	} else {
		throw_err(tokens, max, i, "Invalid symbol", "="); exit(0);
	}

	skip_white_space(tokens, &i);

	if(type == INT_TYPE){
		if(tokens[i].type == IDENTIFIER && get_literal_value(tokens[i].word, value) == 0){
			++i;
		} else {
			throw_err(tokens, max, i, "invalid value", "literal value"); exit(0);
		}
	} else {
		i = get_char_value(tokens, max, i, value);
	}

	skip_white_space(tokens, &i);

	if(tokens[i].type == END_SIGN){
		++i;
		return i;
	} else {
		throw_err(tokens, max, i, "syntax error", ";");
		exit(0);
	}
}



ASGMT var_asgmt(TOKEN *tokens, int max, int *cidx){
	ASGMT ae;
	ae.is_str = 0;
	ae.is_func = 0;
	int i = *cidx;
	ae.type = INVALID_TYPE;
	ae.name = NULL;

	if((ae.type = get_type(tokens[i].word)) != INVALID_TYPE){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid type", "int, char, char[]"); exit(0);
	}
 
	skip_white_space(tokens, &i);

	// get varialbe name
	if(tokens[i].type == IDENTIFIER){
		ae.name = (char *)calloc((int)strlen(tokens[i].word) + 1, sizeof(char));
		strcpy(ae.name, tokens[i].word);
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid varialbe name", NULL); exit(0);
	}

	skip_white_space(tokens, &i);

	if(tokens[i].type == BRAKET_OPN){
		ae.str = (char *)calloc(MAXSIZ, sizeof(char));
		i = str_var_asgmt(tokens, max, i, ae.str);
		ae.is_str = 1;
		printf("VALUE: >%s<\n", ae.str);
		return ae;

	} else if(strcmp(tokens[i].word, "(") == 0){
		// function

	} else if(strcmp(tokens[i].word, "=") == 0){
		// normal variable
		i = u8_var_asgmt(tokens, max, i, &ae.value, ae.type);
		printf("VALUE: >%d<\n", ae.value);
		return ae;

	} else {
		throw_err(tokens, max, i, "Invalid symbol", NULL);
		exit(0);
	}

	return ae;
}
