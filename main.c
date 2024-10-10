#include "src/utility.h"
#include "src/tokenizer.h"
#include "src/parser.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	char buff[MAXFILE];
	read_file(gflag.input, buff);

	TKNS tkns;
	tokenizer(buff, &tkns);  // break down the source code into tokens

	parser(&tkns);  // Parse tokens & check for errors

	// func_exists("main", 1);  // check for function 'main'

	printf("\n\n\n\nINFORMATIONS\n");
	show_global_variables();
	show_functions();
	return 0;
}

