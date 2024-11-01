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


	add_tree("STATUS EQU 0x03  ; Added by Compiler (only for pic10f200)");
	add_tree("Z EQU 0x02 ; Added by Compiler (only for pic10f200)");

	parser(&tkns, 0, &tree_idx, AST_NO_STATEMENT);  // Parse tokens & check for errors


	func_exists("main", 1);  // Check that if function 'main' exists (1 means exit if there is no 'main')

	reorder();  // Reorder functions for assembler
	write_tree(gflag.output);
	return 0;
}

