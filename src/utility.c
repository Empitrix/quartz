#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "types.h"
#include "rules.h"


/* lower_str: make a char* to a lower case char* */
void lower_str(char buff[]){
	int i;
	for(i = 0; buff[i] != '\0'; ++i){
		buff[i] = tolower(buff[i]);
	}
}

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

	strcpy(gflag->target, "pic10f200");

	for(int i = 2; i < argc; ++i){

		char targets[][100] = { "pic10f200", "pic10f202"};  // supported targets
		int target_exists = 0;

		if(sscanf(argv[i], "--target=%s", gflag->target)){
			// Check if there is a valid target
			for(int j = 0; j < (int)(sizeof(targets) / sizeof(targets[1])); ++j){
				lower_str(gflag->target);  // lowercase
				// break the loop if exists
				if(strcmp(gflag->target, targets[j]) == 0){ target_exists = 1; break; }
			}

			// Show error if target does not exists (show all of the supported targets)
			if(target_exists == 0){
				printf("Invalid target \"%s\"\nUse:\n", gflag->target);

				for(int j = 0; j < (int)(sizeof(targets) / sizeof(targets[1])); ++j){
					printf("\t- %s\n", targets[j]);
				}
				exit(0);  // exit
			}
		}

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




/* get_char_value: returns 0 if everything is fine otherwise returns 1 */
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
	}
	return 1;
}


void show_ast_t(ast_t t, int flush){
	switch(t){
	case AST_VARIABLE_ASSIGNMENT:
		printf("VARIABLE_ASSIGNMENT");
		break;
	case AST_FOR_LOOP_ASSIGNMENT:
		printf("FOR_LOOP_ASSIGNEMNT");
		break;
	case AST_WHILE_LOOP_ASSIGNMENT:
		printf("WHILE_LOOP_ASSIGNEMNT");
		break;
	case AST_IF_STATEMENT:
		printf("IF_STATEMENT");
		break;
	case AST_ELSE_STATEMENT:
		printf("ELSE_STATEMENT");
		break;
	case AST_FUNCTION_ASSIGNMENT:
		printf("FUNCTION_ASSIGNEMNT");
		break;
	case AST_RETURN_STATEMENT:
		printf("RETURN_STATEMENT");
		break;
	case AST_FUNCTION_CALL:
		printf("FUNCTION_CALL");
		break;
	case AST_STATEMENT:
		printf("STATEMENT");
		break;
	case AST_NO_STATEMENT:
		printf("NO_STATEMENT");
		break;
	case AST_RAW_ASM:
		printf("RAW_ASM");
		break;
	case AST_MACRO:
		printf("MACRO");
		break;
	}
	if(flush) putchar('\n');
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
	int check = 1;

	char tmp[1000][100];
	int tmp_idx = 0;

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


void add_tree(const char inpt[]){ strcpy(tree[tree_idx++], inpt); }

// void add_to_tree(int *tidx, char inpt[]){
// 	strcpy(tree[*tidx], inpt);
// 	*tidx = *tidx + 1;
// }

// break down lines that contains '\n' e.g. "MOVLW 12\nMOVWF i" to separate lines
void update_tree_lines(char code[]){
	char lines[10][128] = { 0 };
	char temp[128] = { 0 };
	int tidx = 0;
	int pointer = 0;
	int idx = 0;

	while(code[idx] != '\0'){
		temp[tidx++] = code[idx];
		if(code[idx++] == '\n' || code[idx] == '\0' ){
			if(temp[tidx - 1] == '\n'){ tidx--; }
			temp[tidx] = '\0';
			strcpy(lines[pointer++], temp);
			tidx = 0;
		}
	}
	for(int i = 0; i < pointer; i++){
		add_to_tree(lines[i]);
	}
}


const char *ast_f(ast_t t){
	switch(t){
		case AST_VARIABLE_ASSIGNMENT: return "VARIABLE_ASSIGNMENT";
		case AST_FOR_LOOP_ASSIGNMENT: return "FOR_LOOP_ASSIGNEMNT";
		case AST_WHILE_LOOP_ASSIGNMENT: return "WHILE_LOOP_ASSIGNEMNT";
		case AST_IF_STATEMENT: return "IF_STATEMENT";
		case AST_ELSE_STATEMENT: return "ELSE_STATEMENT";
		case AST_FUNCTION_ASSIGNMENT: return "FUNCTION_ASSIGNEMNT";
		case AST_RETURN_STATEMENT: return "RETURN_STATEMENT";
		case AST_FUNCTION_CALL: return "FUNCTION_CALL";
		case AST_STATEMENT: return "STATEMENT";
		case AST_NO_STATEMENT: return "NO_STATEMENT";
		case AST_RAW_ASM: return "RAW_ASM";
		case AST_MACRO: return "MACRO";
	}
	return "";
}

void show_tab(int siz){
	for(int i = 0; i < siz; ++i){
		printf("\t");
	}
}


void show_tree(){
	int i = 0;
	for(i = 0; i < qast_idx; ++i){
		show_tab(qasts[i].indent);
		printf("%s\n", ast_f(qasts[i].type));
	}

	printf("\n\n\n\nRefer:\n");
	for(int i = 0; i < qast_idx; ++i){
		printf("%s -> %s\n", ast_f(qasts[i].type), ast_f(qasts[i].refer));
	}
}


int arg_exists(Qarg args[], int len, Qarg arg){
	for(int i = 0; i < len; ++i){
		if(strcmp(args[i].name, arg.name) == 0){
			return 1;
		}
	}
	return 0;
}

int same_type_arg(Qvar var, Qarg arg){
	if(var.type == CONSTANT_INTEGER && arg.type == QVAR_INT){
		return 1;
	}

	if(var.type == CONSTANT_CHAR && arg.type == QVAR_CHAR){
		return 1;
	}

	return var.type == arg.type;
}


int empty_body(TKNS *tkns){
	int counter = 0;
	for(int i = 0; i < tkns->max; ++i){
		if(tkns->tokens[i].type == WHITESPACE ||
				tkns->tokens[i].type == NEWLINE ||
				tkns->tokens[i].type == COMMENT_TOK){ continue; }
		counter++;
	}
	return counter == 0;
}


int skip_empty_tok(TOKEN t){
	if(t.type == WHITESPACE ||
			t.type == COMMENT_TOK ||
			t.type == END_SIGN ||
			t.type == NEWLINE){
		return 1;
	}
	return 0;
}
