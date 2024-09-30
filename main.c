#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024


typedef struct GFLAG {
	char *output;
} GFLAG;


void update_glfag(GFLAG *gflag, int argc, char *argv[]){
	int get_source = 0;

	for(int i = 1; i < argc; ++i){
		printf("%s\n", argv[i]);
		if(get_source){
			gflag->output = (char *)calloc((int)strlen(argv[i]) + 1, sizeof(char));
			strcpy(gflag->output, argv[i]);
			get_source = 0;
		}
		if(argv[i][0] == '-'){
			for(int j = 1; j < (int)strlen(argv[i]); ++j){
				switch(argv[i][j]){
					case 'o':
						get_source = 1;
						break;
					default:
						break;
				}
			}
		}
	}
	if(get_source){
		printf("%s: fatal error: no input files\ncompilation terminated.\n", argv[0]);
		exit(0);
	}
}


char *read_srouce(char *path){
	char *source = (char *)calloc(MAX_LENGTH, sizeof(char));
	printf("PATH: >%s<\n", path);
	return source;
}


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);
	printf("SOURCE FILE: >%s<\n", gflag.output);
	free(gflag.output);
	return 0;
}
