#include "src/global.h"
#include "src/utility.h"
#include "src/tokenizer.h"
#include "src/generator.h"
#include "src/parser.h"
#include <stdio.h>



int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	char buff[MAXFILE] = { 0 };
	read_file(gflag.input, buff);

	TKNS tkns;
	tokenizer(buff, &tkns);  // break down the source code into tokens


	add_tree("STATUS EQU 0x03  ; Added by Compiler (only for pic10f200)");
	add_tree("Z EQU 0x02 ; Added by Compiler (only for pic10f200)");

	parser(&tkns, 0, &tree_idx, AST_NO_STATEMENT);  // Parse tokens & check for errors


	printf("AST Len: %d\n", ast_len());
	for(int i = 0; i < ast_len(); ++i){
		printf("%d%-3s", i + 1, ". ");
		show_ast_info(asts[i]);
	}

	func_exists("main", 1);  // Check that if function 'main' exists (1 means exit if there is no 'main')

	generator();

	reorder();  // Reorder functions for assembler

	write_tree(gflag.output);
	return 0;
}


