#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "types.h"
#include "rules.h"


/* Setup General Flags */
void update_glfag(GFLAG *gflag, int argc, char *argv[]){
	if(argc == 1){
		printf("%s: fatal error: no input files\ncompilation terminated.\n", argv[0]);
		exit(0);
	}

	if(access(argv[1], F_OK) == 0) {
		strcpy(gflag->input, argv[1]);
	} else {
		printf("Invalid input file: %s\ncompilation terminated.\n", argv[1]);
		exit(0);
	}

	int get_output = 0;

	for(int i = 2; i < argc; ++i){
		if(get_output){
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
		exit(0);
	}

	if(strcmp(gflag->output, "") == 0){ strcpy(gflag->output, "./out"); }
}


/* read file by given path */
void read_file(char *path, char buff[]){
	FILE *fp = fopen(path, "r");
	int idx = 0;
	int letter = EOF;
	while((letter = fgetc(fp)) != EOF){
		buff[idx++] = letter;
	}
	buff[idx] = '\0';
	fclose(fp);
}


void write_tree(char *path){
	FILE *fp = fopen(path, "w+");
	for(int i = 0; i < tree_idx; ++i){
		fprintf(fp, "%s\n", tree[i]);
	}
	fclose(fp);
}


/* get_token_line: get the whole line that invalid token was found in */
void get_token_line(TKNS *tkns, int *new_start, char line[]){
	int occurred = tkns->idx;

	while(tkns->tokens[occurred].type == NEWLINE && occurred > 0){
		occurred--;
	}

	int tmp = tkns->tokens[occurred].row;
	if(new_start != NULL){ *new_start = occurred; }

	do {
		occurred--;
		if(occurred < 0){
			break;
		}
	} while(tkns->tokens[occurred].row == tmp);

	do {
		if((occurred++) >= tkns->max){ break; }
		if(strcmp(tkns->tokens[occurred].word, "\n") != 0){
			strcat(line, tkns->tokens[occurred].word);
		}
	} while(tkns->tokens[occurred].row == tmp);
}


/* throw_err: show error message with line/line number and underlined invalid token */
void throw_err(TKNS *tkns, const char *msg, const char *expected){
	int new_start = 0;
	char line[MAXSIZ];
	char obj[MAXSIZ];
	memset(obj, '\0', sizeof(obj));
	memset(line, '\0', sizeof(line));
	char output[MAXSIZ];
	get_token_line(tkns, &new_start, line);

	int i;
	int occurred = new_start;
	int tok_len = tkns->tokens[occurred].col;
	strcat(obj, "\033[38;2;230;81;0m");
	for(i = 0; i < tok_len; ++i){
		if(i == tok_len - 1){
			strcat(obj, "^");
		} else {
			strcat(obj, "~");
		}
	}
	strcat(obj, "\033[0m");
	if(expected != NULL && strcmp(expected, "") != 0){
		sprintf(output, "%s, expected (%s) in", msg, expected);
	} else {
		sprintf(output, "%s", msg);
	}
	printf("Err: %s:\n\n%-3d|%s\n   |%s\n   |\n\n", output, tkns->tokens[occurred].row, line, obj);
	exit(0);
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

			if(rt > 255){ return 1; }  // check if the number is 8-bit
			return 0;  // fine
		}
	}
	return 1;  // failed
}



int get_string(TKNS *tkns, char src[]){
	if(tkns->tokens[tkns->idx].type == WHITESPACE){ ++tkns->idx; }

	if(tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
		++tkns->idx;
	} else {
		throw_err(tkns, "Invalid syntax", "\"");
		exit(0);
	}

	do {
		if(tkns->tokens[tkns->idx].type == DOUBLE_QUOTE){
			return 0;
		}
		strcat(src, tkns->tokens[tkns->idx].word);
		++tkns->idx;
		if(tkns->tokens[tkns->idx].type == NEWLINE){
			throw_err(tkns, "Invalid syntax", "\"");
			exit(0);
		}
	} while(1);

	return 1;
}




int get_char_value(TKNS *tkns, char *value){
	if(tkns->tokens[tkns->idx].type == WHITESPACE){ tkns->idx++; }

	if(tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		tkns->idx++;
	} else {
		throw_err(tkns, "Invalid syntax", "'");
		exit(0);
	}

	int escape = 0;
	char letter = 0;

	if(strcmp(tkns->tokens[tkns->idx].word, "\\") == 0){
		escape = 1;
		tkns->idx++;
	} else {
		letter = tkns->tokens[tkns->idx].word[0];
		tkns->idx++;
	}

	if(escape){
		if(strcmp(tkns->tokens[tkns->idx].word, "n") == 0){
			letter = '\n';
			tkns->idx++;
		} else if(strcmp(tkns->tokens[tkns->idx].word, "t") == 0){
			letter = '\t';
			tkns->idx++;
		} else if(strcmp(tkns->tokens[tkns->idx].word, "\\") == 0){
			letter = '\\';
			tkns->idx++;
		} else {
			throw_err(tkns, "Invalid escape letter", "\\n, \\t, \\\\");
			exit(0);
		}
	}


	if(tkns->tokens[tkns->idx].type == SINGLE_QUOTE){
		*value = (int)letter;
		tkns->idx++;
		return 0;
	} else {
		throw_err(tkns, "Invalid syntax", "'");
		exit(0);
	}
	return 1;
}




void type_to_str(var_t type, char dst[]){
	switch (type) {
		case INT_VAR: strcpy(dst, "int"); break;
		case CHAR_VAR: strcpy(dst, "char"); break;
		case STR_VAR: strcpy(dst, "char[]"); break;
	}
}




void convert_var_to_const(VAR *var, CNST_VAR *cnst){
	cnst->type = var->type;
	strcpy(cnst->name, var->name);

	switch(var->type){
		case INT_VAR:
			cnst->int_value = var->value;
			break;

		case CHAR_VAR:
			cnst->char_value = var->value;
			break;

		case STR_VAR:
			strcpy(cnst->str_value, var->str_value);
			break;
	}

}


int get_arighmetic(TKNS *tkns){
	token_t tmp = WHITESPACE;

	if(tkns->tokens[tkns->idx].type == PLUS_SIGN || tkns->tokens[tkns->idx].type == MINUS_SIGN){
		tkns->idx++;
		tmp = tkns->tokens[tkns->idx].type;
		if(tkns->tokens[tkns->idx].type == tmp){
			if(tkns->tokens[tkns->idx].type == PLUS_SIGN){
				tkns->idx++;
				return 1;
			} else {
				tkns->idx++;
				return -1;
			}
		} else {
			throw_err(tkns, "Invalid arithmetic symbols", "++ or --");
			exit(0);
		}
	}

	return 0;
}


void show_ast_info(AST ast){
	switch(ast.type){
	case AST_VARIABLE_ASSIGNMENT:
		printf("AST_VARIABLE_ASSIGNEMNT");
		break;
	case AST_FOR_LOOP_ASSIGNMENT:
		printf("AST_FOR_LOOP_ASSIGNEMNT");
		break;
	case AST_WHILE_LOOP_ASSIGNMENT:
		printf("AST_WHILE_LOOP_ASSIGNEMNT");
		break;
	case AST_IF_STATEMENT:
		printf("AST_IF_STATEMENT");
		break;
	case AST_ELSE_STATEMENT:
		printf("AST_ELSE_STATEMENT");
		break;
	case AST_FUNCTION_ASSIGNMENT:
		printf("AST_FUNCTION_ASSIGNEMNT");
		break;
	case AST_RETURN_STATEMENT:
		printf("AST_RETURN_STATEMENT");
		break;
	case AST_FUNCTION_CALL:
		printf("AST_FUNCTION_CALL");
		break;
	case AST_STATEMENT:
		printf("AST_STATEMENT");
		break;
	case AST_NO_STATEMENT:
		printf("AST_NO_STATEMENT");
		break;
	}
	putchar('\n');
}


/* strcatf: formated strcat */
void strcatf(char* dst, const char * frmt, ...){
	char tmp[MAXSIZ];
	va_list arglist;
	va_start(arglist, frmt);
	vsprintf(tmp, frmt, arglist);
	va_end(arglist);
	strcat(dst, tmp);
}


/* strcnt: string contains */
int strcnt(const char *src, const char *inpt) {
	if (src == NULL || inpt == NULL) { return 0; }
	return strstr(src, inpt) != NULL;
}



void shift_tree(int start, int end) {
	// if (start < 0 || end >= tree_idx || start > end) { return; }

	int shift_count = end - start + 1;

	for (int i = end + 1; i < tree_idx; i++) {
		strcpy(tree[i - shift_count], tree[i]);
	}

	tree_idx -= shift_count;

	for (int i = tree_idx; i < tree_idx + shift_count; i++) {
		tree[i][0] = '\0';
	}
}


void reorder(void){
	int start = 0;
	int end = 0;
	int main_found = 0;
	int reached = 0;
	int check = 1;

	char tmp[1000][100];
	int tmp_idx = 0;

	int counter = 0;
	while(check){
		start = -1;
		end = -1;

		for(int i = 0; i < tree_idx; ++i){
			// a function except 'main'
			if(strcnt(tree[i], ":") && strcnt(tree[i], "main") == 0 && start == -1){ start = i; }
			if(strcnt(tree[i], "RETLW") && end == -1 && start != -1){ end = i; break; }
		}

		if(start == -1 || end == -1){ check = 0; break; }


		// copy to tmp buff
		for(int i = start; i < end + 1; ++i){ strcpy(tmp[tmp_idx++], tree[i]); }
		shift_tree(start, end);

	}

	// copy the tmp at the end of the tree
	for(int i = 0; i < tmp_idx; ++i){ strcpy(tree[tree_idx++], tmp[i]); }
}

