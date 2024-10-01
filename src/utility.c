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
		free_gflag(gflag);
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


Token* tokenize(char *inpt, int *count){
	int size = 10;
	Token *tokens = (Token*)calloc(size, sizeof(Token));

	int i = 0;
	int row = 1;
	int col = 1;

	while(inpt[i]){
		int scol = col;
		int idx = i;
		char *word = NULL;



		if(inpt[i] == '\t'){
			word = (char*)calloc(3, sizeof(char));
			strcpy(word, "  ");
			i++;
			col++;

		} else if(inpt[i] == '\n'){
			word = (char*)calloc(2, sizeof(char));
			strcpy(word, "\n");
			i++;
			row++;
			col = 1;

		} else if(inpt[i] == ' '){
			while(inpt[i] == ' '){
				i++;
				col++;
			}
			int len = i - idx;
			word = (char*)calloc(len + 1, sizeof(char));
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';

		} else if(isalnum(inpt[i]) || inpt[i] == '_'){
			while(isalnum(inpt[i]) || inpt[i] == '_'){
				i++;
				col++;
			}
			int len = i - idx;
			word = (char*)calloc(len + 1, sizeof(char));
			strncpy(word, &inpt[idx], len);
			word[len] = '\0';

		} else {
			word = (char*)calloc(2, sizeof(char));
			word[0] = inpt[i];
			word[1] = '\0';
			i++;
			col++;
		}

		// int len = i - idx;
		// char *word = (char *)calloc(len + 1, sizeof(char));
		// strncpy(word, &inpt[idx], len);

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
