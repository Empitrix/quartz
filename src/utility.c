#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "rules.h"


/* Free General Flags */
void free_gflag(GFLAG *gflag){
	free(gflag->input);
	gflag->input = NULL;
	free(gflag->output);
	gflag->output = NULL;
}


/* Setup General Flags */
void update_glfag(GFLAG *gflag, int argc, char *argv[]){
	if(argc == 1){
		printf("%s: fatal error: no input files\ncompilation terminated.\n", argv[0]);
		exit(0);
	}

	if(access(argv[1], F_OK) == 0) {
		gflag->input = (char *)calloc((int)strlen(argv[1]) + 1, sizeof(char));
		strcpy(gflag->input, argv[1]);
	} else {
		printf("Invalid input file: %s\ncompilation terminated.\n", argv[1]);
		free_gflag(gflag);
		exit(0);
	}

	int get_output = 0;

	for(int i = 2; i < argc; ++i){
		if(get_output){
			gflag->output = (char *)calloc((int)strlen(argv[i]) + 1, sizeof(char));
			strcpy(gflag->output, argv[i]);
			get_output = 0;
		}
		if(argv[i][0] == '-'){
			for(int j = 1; j < (int)strlen(argv[i]); ++j){
				switch(argv[i][j]){
					case 'o':
						get_output = 1;
						break;
					default:
						break;
				}
			}
		}
	}
	if(get_output){
		printf("%s: no output file!\ncompilation terminated.\n", argv[0]);
		free_gflag(gflag);
		exit(0);
	}
}


/* read file by given path */
char *read_file(char *path){
	char *buff = (char *)calloc(1000, sizeof(char));
	char *line = (char *)calloc(MAXSIZ, sizeof(char));
	FILE *fp = fopen(path, "r");
	while(fgets(line, MAXSIZ, fp) != NULL){
		strcat(buff, line);
	}
	fclose(fp);
	free(line);
	buff = realloc(buff, ((int)strlen(buff) + 1) * sizeof(char));
	return buff;
}


/*
Token* tokenize(char *inpt, int *count){
	int size = 10;
	Token *tokens = (Token*)calloc(size, sizeof(Token));

	int i = 0;
	int row = 1;
	int col = 1;

	while(inpt[i]){
		int scol = col;
		int idx = i;
		if(isspace(inpt[i])){
			while(isspace(inpt[i])){
				if(inpt[i] == '\n'){ row++; col = 1; } else { col++;}
				i++;
			}
		} else if(isalnum(inpt[i]) || inpt[i] == '_'){
			while(isalnum(inpt[i]) || inpt[i] == '_'){
				i++;
				col++;
			}
		} else {
			i++;
			col++;
		}

		int len = i - idx;
		char *word = (char *)calloc(len + 1, sizeof(char));
		strncpy(word, &inpt[idx], len);

		if(*count >= size){
			size *= 2;
			tokens = (Token*)realloc(tokens, size * sizeof(Token));
		}
		tokens[*count].word = word;
		tokens[*count].row = row;
		tokens[*count].col = scol;
		*count = *count + 1;
	}
	return tokens;
}
*/


// Free a list of tokens (pointer)
void free_tokens(TOKEN *tokens, int token_count) {
	for (int i = 0; i < token_count; i++) {
		free(tokens[i].word);
	}
	free(tokens);
}



TOKEN* tokenize(char *inpt, int *count){
	int size = 10;
	TOKEN *tokens = (TOKEN *)calloc(size, sizeof(TOKEN));

	int i = 0;
	int row = 1;
	int col = 1;

	while(inpt[i]){
		int scol = col;
		int idx = i;
		char *word = NULL;

		
		token_t ttype;


		if(inpt[i] == '\t'){
			word = (char*)calloc(3, sizeof(char));
			strcpy(word, "  ");
			i++;
			col++;
			ttype = WHITESPACE;

		} else if(inpt[i] == '\r'){
			continue;

		} else if(inpt[i] == '\n'){
			word = (char*)calloc(2, sizeof(char));
			strcpy(word, "\n");
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
			word = (char*)calloc(len + 1, sizeof(char));
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = WHITESPACE;

		} else if(isalnum(inpt[i]) || inpt[i] == '_'){
			while(isalnum(inpt[i]) || inpt[i] == '_'){
				i++;
				col++;
			}
			int len = i - idx;
			word = (char*)calloc(len + 1, sizeof(char));
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';
			ttype = IDENTIFIER;

		} else {
			word = (char*)calloc(2, sizeof(char));
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

		if(*count >= size){
			size *= 2;
			tokens = (TOKEN*)realloc(tokens, size * sizeof(TOKEN));
		}

		tokens[*count].word = word;
		tokens[*count].row = row;
		tokens[*count].col = scol;
		tokens[*count].type = ttype;
		*count = *count + 1;
	}
	return tokens;
}


/* get_token_line: get the whole line that invalid token was found in */
char *get_token_line(TOKEN *tokens, int max, int occurred, int *new_start){
	char *line = (char *)calloc(MAXSIZ, sizeof(char));

	while(tokens[occurred].type == NEWLINE && occurred > 0){
		occurred--;
	}

	int tmp = tokens[occurred].row;
	if(new_start != NULL){ *new_start = occurred; }

	do {
		occurred--;
		if(occurred < 0){
			break;
		}
	} while(tokens[occurred].row == tmp);

	do {
		if((occurred++) >= max){ break; }
		if(strcmp(tokens[occurred].word, "\n") != 0){
			strcat(line, tokens[occurred].word);
		}
	} while(tokens[occurred].row == tmp);
	return line;
}


/* throw_err: show error message with line/line number and underlined invalid token */
void throw_err(TOKEN *tokens, int max, int occurred, char *msg, char *expected){
	int new_start = 0;
	char *line = get_token_line(tokens, max, occurred, &new_start);
	char *obj = (char *)calloc(MAXSIZ, sizeof(char));
	char *output = (char *)calloc(MAXSIZ, sizeof(char));
	int i;
	occurred = new_start;
	int tok_len = tokens[occurred].col;
	strcat(obj, "\e[38;2;230;81;0m");
	for(i = 0; i < tok_len; ++i){
		if(i == tok_len - 1){
			strcat(obj, "^");
		} else {
			strcat(obj, "~");
		}
	}
	strcat(obj, "\e[0m");
	if(expected != NULL && strcmp(expected, "") != 0){
		sprintf(output, "%s, expexted (%s) in", msg, expected);
	} else {
		sprintf(output, "%s", msg);
	}
	printf("Err: %s:\n\n%-3d|%s\n   |%s\n   |\n\n", output, tokens[occurred].row, line, obj);
	free(output);
	free(obj);
	free(line);
}




int get_literal_value(char *word, int *number){
	char *leftover;
	int rt = 0;
	int diff[] = {2, 10, 16};
	char *left[3] = {"B", "", ""};

	for(int i = 0; i < 3; ++i){
		rt = strtol(word, &leftover, diff[i]);
		if(strcmp(leftover, left[i]) == 0){
			*number = rt;
			return 0;  // fine
		}
	}
	return 1;  // failed
}



int get_string(TOKEN *tokens, int max, int occurred, char *src, int *idx){
	char *output = (char *)calloc(MAXSIZ, sizeof(char));
	int i = occurred;

	if(tokens[i].type == WHITESPACE){ ++i; }

	if(tokens[i].type == DOUBLE_QUOTE){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", "\"");
		exit(0);
	}

	do {
		if(tokens[i].type == DOUBLE_QUOTE){
			strcpy(src, output);
			free(output);
			*idx = i;
			return 0;
		}
		strcat(output, tokens[i].word);
		++i;
		if(tokens[i].type == NEWLINE){
			throw_err(tokens, max, i, "Invalid syntax", "\"");
			exit(0);
		}
	} while(1);

	return 1;
}




int get_char_value(TOKEN *tokens, int max, int i, int *value){
	if(tokens[i].type == WHITESPACE){ ++i; }

	if(tokens[i].type == SINGLE_QUOTE){
		++i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", "'");
		exit(0);
	}

	int escape = 0;
	char letter = 0;

	if(strcmp(tokens[i].word, "\\") == 0){
		escape = 1;
		++i;
	} else {
		letter = tokens[i].word[0];
		++i;
	}

	if(escape){
		if(strcmp(tokens[i].word, "n") == 0){
			letter = '\n';
			++i;
		} else if(strcmp(tokens[i].word, "t") == 0){
			letter = '\t';
			++i;
		} else if(strcmp(tokens[i].word, "\\") == 0){
			letter = '\\';
			++i;
		} else {
			throw_err(tokens, max, i, "Invalid escape letter", "\\n, \\t, \\\\");
			exit(0);
		}
	}


	if(tokens[i].type == SINGLE_QUOTE){
		*value = (int)letter;
		++i;
		return i;
	} else {
		throw_err(tokens, max, i, "Invalid syntax", "'");
		exit(0);
	}

	return i;
}

