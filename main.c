#include "src/utility.h"
#include "src/tokenizer.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	char buff[MAXFILE];
	read_file(gflag.input, buff);

	Tokens tokens;
	tokenizer(buff, &tokens);

	// parser(tokens, tlen);

	return 0;
}

