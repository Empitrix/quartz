#include "src/global.h"
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

	parser(&tkns, 0, &tree_idx);  // Parse tokens & check for errors

	func_exists("main", 1);  // Check that if function 'main' exists

	strcpy(tree[tree_idx++], "\tSLEEP");  // sleep at the end of the code

	write_tree(gflag.output);


	return 0;
}

