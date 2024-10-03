#include "src/types.h"
#include "src/utility.h"
#include "src/parser.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);
	char *buff = read_file(gflag.input);

	int tlen = 0;
	TOKEN *tokens = tokenize(buff, &tlen);

	parser(tokens, tlen);

	
	// FREE
	free(buff);
	free_gflag(&gflag);
	free_tokens(tokens, tlen);
	return 0;
}

