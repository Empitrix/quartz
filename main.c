#include "src/utility.h"
#include "src/lexer.h"
#include "src/generator.h"
#include "src/parser.h"


int main(int argc, char *argv[]){
	set_compiler_diag_exit(1);

	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	// Read Input File
	char buff[MAXFILE] = { 0 };
	read_file(gflag.input, buff);

	//
	TKNS tkns;
	tokenizer(buff, &tkns);  // break down the source code into tokens


	if(strcmp(gflag.target, "pic10f200") == 0){
		add_tree("STATUS EQU 0x03  ; Added by Compiler (only for pic10f200)");
		add_tree("Z EQU 0x02       ; Added by Compiler (only for pic10f200)");
		add_tree("C EQU 0x00       ; Added by Compiler (only for pic10f200)");
	}
	add_tree("CRAM EQU 0x19    ; Compiler Reserved Address");

	// Parse TKNS
	qparser(&tkns, 0, AST_NO_STATEMENT);
	update_children();

	// Check function main exists
	if(qfunc_exists("main") != 1){
		printf("Function \"main\" does not exists!\n");
		exit(0);
	}

	generator(qasts, 0, qast_idx);  // Generate Assembley code
	reorder();                      // Reorder functions for linker

	// Flags
	if(gflag.parser_view){ visualize_tree(); }
	if(gflag.lexer_view){ show_lexer(&tkns); }
	if(gflag.gen_view){ show_asm_gen(); }

	// Generate binary using linker
	write_tree(COMPILE_NAME);
	linker(&gflag);
	return 0;
}


