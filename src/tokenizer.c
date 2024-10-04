#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "types.h"


/* tokenizer: Make tokens form given string */
void tokenizer(char inpt[], TKNS *tokens){
	int i = 0;
	int row = 1;
	int col = 1;

	tokens->max = 0;
	tokens->idx = 0;

	while(inpt[i]){
		int scol = col;
		int idx = i;
		char word[32];
		token_t ttype;

		if(inpt[i] == '\t'){
			word[0] = ' ';
			word[1] = ' ';
			word[2] = '\0';
			i++;
			col++;
			ttype = WHITESPACE;

		} else if(inpt[i] == '\r'){
			continue;

		} else if(inpt[i] == '\n'){
			word[0] = '\n';
			word[0] = '\0';
			i++;
			row++;
			col = 1;
			ttype = NEWLINE;

		} else if(inpt[i] == ' '){
			while(inpt[i] == ' '){
				i++;
				col++;
			}
			int len = i - idx;
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = WHITESPACE;

		} else if(isalnum(inpt[i]) || inpt[i] == '_'){
			while(isalnum(inpt[i]) || inpt[i] == '_'){
				i++;
				col++;
			}
			int len = i - idx;
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = IDENTIFIER;

		} else {
			word[0] = inpt[i];
			word[1] = '\0';
			i++;
			col++;
			if(strcmp(word, "=") == 0){
				ttype = EQUAL_SIGN;
			} else if(strcmp(word, "[") == 0){
				ttype = BRAKET_OPN;
			} else if(strcmp(word, "]") == 0){
				ttype = BRAKET_CLS;
			} else if(strcmp(word, "(") == 0){
				ttype = PAREN_OPN;
			} else if(strcmp(word, ")") == 0){
				ttype = PAREN_CLS;
			} else if(strcmp(word, "{") == 0){
				ttype = BRACE_OPN;
			} else if(strcmp(word, "}") == 0){
				ttype = BRACE_CLS;
			} else if(strcmp(word, ";") == 0){
				ttype = END_SIGN;
			} else if(strcmp(word, "'") == 0){
				ttype = SINGLE_QUOTE;
			} else if(strcmp(word, "\"") == 0){
				ttype = DOUBLE_QUOTE;
			} else {
				ttype = UNKNOWN;
			}
		}

		TOKEN tkn;
		strcpy(tkn.word, word);
		tkn.row = row;
		tkn.col = scol;
		tkn.type = ttype;

		tokens->tokens[tokens->max] = tkn;
		tokens->max++;
	}
}
