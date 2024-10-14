#include "src/utility.h"
#include "src/tokenizer.h"
#include "src/parser.h"
#include <stdio.h>


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	char buff[MAXFILE];
	read_file(gflag.input, buff);

	TKNS tkns;
	tokenizer(buff, &tkns);  // break down the source code into tokens

	parser(&tkns, 0, &tree_idx);  // Parse tokens & check for errors

	strcpy(tree[tree_idx++], "\tSLEEP");  // sleep at the end of the code

	write_tree(gflag.output);

	// printf("TREE --->\n");
	// for(int i = 0; i < tree_idx; ++i){
	// 	printf("%s\n", tree[i]);
	// }
	// printf("<--- TREE\n");

	// func_exists("main", 1);  // check for function 'main'
	// printf("\n\n\n\nINFORMATIONS\n");
	// show_global_variables();
	// show_functions();
	return 0;
}

