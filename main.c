#include <stdio.h>
#include "src/types.h"
#include "src/utility.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);
	printf("INPUT  >%s<\n", gflag.input);
	printf("OUTPUT >%s<\n", gflag.input);
	char *buff;
	buff = read_file(gflag.input);
	printf(">%s<\n", buff);
	free(buff);
	free_gflag(&gflag);
	return 0;
}

