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

	int prot = 0;  // protected? (" or ')

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


		} else if(inpt[i + 1] && inpt[i] == '/' && inpt[i + 1] == '/' && prot == 0){
			while(inpt[i] != '\n'){
				i++;
				col++;
			}
			int len = i - idx;
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = COMMENT_TOK;

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

		} else if(isdigit(inpt[i])){
			while(isdigit(inpt[i]) || ((inpt[i] >= 'A' && inpt[i] <= 'F') || inpt[i] == 'x')){
				i++;
				col++;
			}
			int len = i - idx;
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = INTEGER_VALUE;

		} else if(isalnum(inpt[i]) || inpt[i] == '_'){
			while(isalnum(inpt[i]) || inpt[i] == '_'){
				i++;
				col++;
			}
			int len = i - idx;
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			if(strcmp(word, "define") == 0){
				ttype = DEFINE_KEWORD;
			} else if(strcmp(word, "include") == 0){
				ttype = INCLUDE_KEWORD;
			} else if(strcmp(word, "if") == 0){
				ttype = IF_KEWORD;
			} else if(strcmp(word, "else") == 0){
				ttype = ELSE_KEWORD;
			} else if(strcmp(word, "for") == 0){
				ttype = FOR_KEWORD;
			} else if(strcmp(word, "while") == 0){
				ttype = WHILE_KEWORD;
			} else if(strcmp(word, "return") == 0){
				ttype = RETURN_KEWORD;
			} else if(strcmp(word, "int") == 0){
				ttype = INT_KEYWORD;
			} else if(strcmp(word, "char") == 0){
				ttype = CHAR_KEYWORD;
			} else {
				ttype = IDENTIFIER;
			}

		} else {
			word[0] = inpt[i];
			word[1] = '\0';
			i++;
			col++;
			switch (word[0]) {
				case '=':
					ttype = EQUAL_SIGN;
					break;
				case ',':
					ttype = COMMA_SIGN;
					break;
				case '[':
					ttype = BRAKET_OPN;
					break;
				case ']':
					ttype = BRAKET_CLS;
					break;
				case '(':
					ttype = PAREN_OPN;
					break;
				case ')':
					ttype = PAREN_CLS;
					break;
				case '{':
					ttype = BRACE_OPN;
					break;
				case '}':
					ttype = BRACE_CLS;
					break;
				case ';':
					ttype = END_SIGN;
					break;
				case '\'':
					ttype = SINGLE_QUOTE;
					prot = ~prot;
					break;
				case '"':
					prot = ~prot;
					ttype = DOUBLE_QUOTE;
					break;
				case '#':
					ttype = HASHTAG;
					break;
				case '<':
					ttype = LEFT_SIGN;
					break;
				case '>':
					ttype = RIGHT_SIGN;
					break;
				case '+':
					ttype = PLUS_SIGN;
					break;
				case '-':
					ttype = MINUS_SIGN;
					break;
				case '~':
					ttype = TILDE_SIGN;
					break;
				case '^':
					ttype = CARET_SIGN;
					break;
				case '&':
					ttype = AND_SIGN;
					break;
				case '|':
					ttype = OR_SIGN;
					break;
				default:
					ttype = UNKNOWN;
					break;
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
