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

	gflag->gen_view = 0;
	gflag->lexer_view = 0;
	gflag->parser_view = 0;
	gflag->delete = 1;
	gflag->linker_verbose = 0;

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
					case 'L':
						gflag->lexer_view = 1;
						break;
					case 'G':
						gflag->gen_view = 1;
						break;
					case 'P':
						gflag->parser_view = 1;
						break;
					case 'v':
						gflag->linker_verbose = 1;
						break;
					case 'U':
						gflag->delete = 1;
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

	char tmp[1000][100] = { 0 };
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
// void update_tree_lines(char code[]){
// 	char lines[10][128] = { 0 };
// 	char temp[128] = { 0 };
// 	int tidx = 0;
// 	int pointer = 0;
// 	int idx = 0;
// 
// 	while(code[idx] != '\0'){
// 		temp[tidx++] = code[idx];
// 		if(code[idx++] == '\n' || code[idx] == '\0' ){
// 			if(temp[tidx - 1] == '\n'){ tidx--; }
// 			temp[tidx] = '\0';
// 			strcpy(lines[pointer++], temp);
// 			tidx = 0;
// 		}
// 	}
// 	for(int i = 0; i < pointer; i++){
// 		add_to_tree(lines[i]);
// 	}
// }


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

const char *ast_str(ast_t t){
	switch(t){
		case AST_VARIABLE_ASSIGNMENT: return "variable assignment";
		case AST_FOR_LOOP_ASSIGNMENT: return "for loop";
		case AST_WHILE_LOOP_ASSIGNMENT: return "while loop";
		case AST_IF_STATEMENT: return "if";
		case AST_ELSE_STATEMENT: return "else";
		case AST_FUNCTION_ASSIGNMENT: return "function assignemnt";
		case AST_RETURN_STATEMENT: return "return";
		case AST_FUNCTION_CALL: return "function call";
		case AST_STATEMENT: return "statement";
		case AST_NO_STATEMENT: return "<EMPTY>";
		case AST_RAW_ASM: return "asm";
		case AST_MACRO: return "macro";
	}
	return "";
}

/* visualize_tree: visualize AST tree */
void visualize_tree(void){
	int i;

	// for(i = 0; i < qast_idx; ++i){
	// 	printf("%s -> %s\n", ast_str(qasts[i].type), ast_str(qasts[i].refer));
	// }

	for(i = 0; i < qast_idx; ++i){
		for (int j = 0; j < qasts[i].depth; j++) {
			printf("│   ");
		}

		if(qasts[i].depth == 0 && i == 0){
			printf("┌── %s (%d)\n", ast_str(qasts[i].type), qasts[i].children);
		} else if(qasts[i].depth > qasts[i + 1].depth){
			printf("└── %s (%d)\n", ast_str(qasts[i].type), qasts[i].children);
		} else {
			printf("├── %s (%d)\n", ast_str(qasts[i].type), qasts[i].children);
		}
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

	// beacuse it's 8 bit
	if(var.type == QVAR_INT && arg.type == QVAR_CHAR){
		return 1;
	}

	return var.type == arg.type;
}


int empty_body(TKNS *tkns){
	for(int i = 0; i < tkns->max; ++i){
		if(tkns->tokens[i].type != WHITESPACE &&
				tkns->tokens[i].type != NEWLINE &&
				tkns->tokens[i].type != COMMENT_TOK){
			return 0;
		}
	}
	return 1;
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


int qvar_defined(Qvar *v){
	if(v->type == QVAR_INT || v->type == QVAR_CHAR){
		return 1;
	}
	return 0;
}



static char arg_names[256][NAME_MAX];
static int arg_name_idx = 0;

int assign_name_exists(char *name){
	for(int i = 0; i < arg_name_idx; ++i){
		if(strcmp(arg_names[i], name) == 0){
			return 1;
		}
	}
	return 0;
}

void assign_var(char *name, int addr){
	if(assign_name_exists(name)){ return; }
	strcpy(arg_names[arg_name_idx++], name);
	attf("\t%s EQU 0x%.2X", name, addr);
}



/* convert token type to string */
const char *tknstr(token_t *t){
	switch(*t){
		case INT_KEYWORD:      return "INT_KEYWORD";
		case CHAR_KEYWORD:     return "CHAR_KEYWORD";
		case IDENTIFIER:       return "IDENTIFIER";
		case INTEGER_VALUE:    return "INTEGER_VALUE";
		case EQUAL_SIGN:       return "EQUAL_SIGN";
		case SEMICOLON_SIGN:   return "SEMICOLON_SIGN";
		case END_SIGN:         return "END_SIGN";
		case BRAKET_OPN:       return "BRAKET_OPN";
		case BRAKET_CLS:       return "BRAKET_CLS";
		case BRACE_OPN:        return "BRACE_OPN";
		case BRACE_CLS:        return "BRACE_CLS";
		case PAREN_OPN:        return "PAREN_OPN";
		case PAREN_CLS:        return "PAREN_CLS";
		case SINGLE_QUOTE:     return "SINGLE_QUOTE";
		case DOUBLE_QUOTE:     return "DOUBLE_QUOTE";
		case HASHTAG:          return "HASHTAG";
		case WHITESPACE:       return "WHITESPACE";
		case NEWLINE:          return "NEWLINE";
		case TYPE_KEYWORDS:    return "TYPE_KEYWORDS";
		case INCLUDE_KEYWORD:  return "INCLUDE_KEYWORD";
		case DEFINE_KEYWORD:   return "DEFINE_KEYWORD";
		case IF_KEYWORD:       return "IF_KEYWORD";
		case ELSE_KEYWORD:     return "ELSE_KEYWORD";
		case RETURN_KEYWORD:   return "RETURN_KEYWORD";
		case VOID_KEYWORD:     return "VOID_KEYWORD";
		case GOTO_KEYWORD:     return "GOTO_KEYWORD";
		case FOR_KEYWORD:      return "FOR_KEYWORD";
		case WHILE_KEYWORD:    return "WHILE_KEYWORD";
		case COMMENT_TOK:      return "COMMENT_TOK";
		case LEFT_SIGN:        return "LEFT_SIGN";
		case RIGHT_SIGN:       return "RIGHT_SIGN";
		case PLUS_SIGN:        return "PLUS_SIGN";
		case MINUS_SIGN:       return "MINUS_SIGN";
		case TILDE_SIGN:       return "TILDE_SIGN";
		case CARET_SIGN:       return "CARET_SIGN";
		case AND_SIGN:         return "AND_SIGN";
		case OR_SIGN:          return "OR_SIGN";
		case EXCLAMATION_SIGN: return "EXCLAMATION_SIGN";
		case COMMA_SIGN:       return "COMMA_SIGN";
		case BACKTICK_SIGN:    return "BACKTICK_SIGN";
		case UNKNOWN:          return "UNKNOWN";
	}
}


/* show lexer output */
void show_lexer(TKNS *tkns){
	for(tkns->idx = 0; tkns->idx < tkns->max; tkns->idx++){
		TOKEN *t = &tkns->tokens[tkns->idx];
		// printf("[%-2d, %-2d]: %s: %-20s\"%s\"\n", t->row, t->col, "", tknstr(&t->type), t->word);
		printf("[%-2d,%2d] - %s: \"%s\"\n", t->row, t->col, tknstr(&t->type), t->word);
	}
	tkns->idx = 0;
}


/* show generated assembly */
void show_asm_gen(){
	for(int i = 0; i < tree_idx; ++i){
		printf("%s\n", tree[i]);
	}
}

/* execute command (save result in 'output')*/
void exec_cmd(char *output, const char *frmt, ...){
	char buff[1024];
	va_list arglist;
	va_start(arglist, frmt);
	vsprintf(buff, frmt, arglist);
	va_end(arglist);

	FILE *fp = popen(buff, "r");
	if(fp == NULL){ return; }
	while (fgets(output, sizeof(output), fp) != NULL);
	pclose(fp);
}

/* generate binary using linker */
void linker(GFLAG *gflag){
	char buff[1024] = { 0 };
	exec_cmd(buff, "./linker %s -v -o %s", COMPILE_NAME, gflag->output);

	if(gflag->linker_verbose){
		printf("%s\n", buff);
	}

	if(gflag->delete){
		char dbuff[1024] = { 0 };
		sprintf(dbuff, "rm %s", COMPILE_NAME);
		system(dbuff);
	}
}
