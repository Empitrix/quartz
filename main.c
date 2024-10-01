#include <stdio.h>
#include "src/types.h"
#include "src/utility.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);
	char *buff = read_file(gflag.input);

	int count = 0;
	Token *tokes = tokenize(buff, &count);

	for(int i = 0; i < count; ++i){
		printf(">%s< %20d, %d\n", tokes[i].word, tokes[i].col, tokes[i].row);
	}

	// FREE
	free(buff);
	free_gflag(&gflag);
	return 0;
}

