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

